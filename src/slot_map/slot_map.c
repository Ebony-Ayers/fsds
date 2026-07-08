#include "slot_map.h"

#define FSDS_SLOT_MAP_INDEX_MASK     0x000000FFFFFFFFFF
#define FSDS_SLOT_MAP_GENCOUNT_MASK  0xFFFFFF0000000000
#define FSDS_SLOT_MAP_GENCOUNT_INCREMENT 0x0000010000000000

#define FSDS_SLOT_MAP_NPOS 0x000000FFFFFFFFFF

static inline FSDS_SlotMapLookupLinkedListNode* FSDS_SlotMap_getLookupList(const FSDS_SlotMap slotMap)
{
    return (FSDS_SlotMapLookupLinkedListNode*)slotMap.memBlock->data;
}

static int FSDS_SlotMap_reallocate(FSDS_SlotMap* const slotMap, size_t newCapacity)
{
    FSDS_SlotMapBlock* oldMemBlock = slotMap->memBlock;
    FSDS_SlotMapHeader* oldHeader = &oldMemBlock->header;

    //compute the padding required to have the data aligned to FSDS_SLOT_MAP_CACHE_LINE_SIZE
    const size_t combinedLookupSize = newCapacity * (sizeof(FSDS_SlotMapLookupLinkedListNode) + sizeof(size_t));
    //round combinedLookupSize up to the nearest FSDS_SLOT_MAP_CACHE_LINE_SIZE
    const size_t combinedLookupSizeWithPadding = (combinedLookupSize + FSDS_SLOT_MAP_CACHE_LINE_SIZE - 1) & ~(FSDS_SLOT_MAP_CACHE_LINE_SIZE - 1);

    //round that data up to the FSDS_SLOT_MAP_CACHE_LINE_SIZE so by construction the alocation is a multiple of FSDS_SLOT_MAP_CACHE_LINE_SIZE.
    //This is a requirement of aligned_alloc
    const size_t roundedDataSize = ((newCapacity * oldHeader->elementSize) + FSDS_SLOT_MAP_CACHE_LINE_SIZE - 1) & ~(FSDS_SLOT_MAP_CACHE_LINE_SIZE - 1);

    //use offsetof instead of sizeof as the c allows sizeof to do funky things and we just want the size of the header and the padding after it before the flexible array member 
    const size_t alocationSize = offsetof(FSDS_SlotMapBlock, data) + combinedLookupSizeWithPadding + roundedDataSize;
    slotMap->memBlock = (FSDS_SlotMapBlock*)aligned_alloc(FSDS_SLOT_MAP_CACHE_LINE_SIZE, alocationSize);
    if(slotMap->memBlock == nullptr)
    {
        return FSDS_SLOT_MAP_ERROR_ALOCATION_FAILED;
    }
    else
    {
        FSDS_SlotMapHeader* header = &slotMap->memBlock->header;
        header->size = oldHeader->size;
        header->capacity = newCapacity;
        header->nextLookup = oldHeader->nextLookup;
        *(size_t*)&header->elementSize = oldHeader->elementSize;
        header->reverseLookupList = (size_t*)(slotMap->memBlock->data + (newCapacity * sizeof(FSDS_SlotMapLookupLinkedListNode)));
        header->dataList = (void*)(slotMap->memBlock->data + combinedLookupSizeWithPadding);

        memcpy(FSDS_SlotMap_getLookupList(*slotMap), oldMemBlock->data, oldHeader->capacity * sizeof(FSDS_SlotMapLookupLinkedListNode));
        memcpy(header->reverseLookupList, oldHeader->reverseLookupList, oldHeader->size * sizeof(size_t));
        memcpy(header->dataList, oldHeader->dataList, oldHeader->size * oldHeader->elementSize);

        free(oldMemBlock);
    }

    return 0;
}

int FSDS_SlotMap_constructDefault(FSDS_SlotMap* const slotMap, const size_t elementSize)
{
    return FSDS_SlotMap_constructSize(slotMap, FSDS_SLOT_MAP_DEFAULT_CAPACITY, elementSize);
}
int FSDS_SlotMap_constructSize(FSDS_SlotMap* const slotMap, size_t initialCapacity, const size_t elementSize)
{
    static_assert(sizeof(FSDS_SlotMapLookupLinkedListNode) == 8, "lookup node must be 8 bytes for the index/gen packing to work");

    //stop initialCapacity = 0 from creating an alocation that must be immediately reallocated to use
    if(initialCapacity == 0) { initialCapacity = FSDS_SLOT_MAP_DEFAULT_CAPACITY; }
    
    //compute the padding required to have the data aligned to FSDS_SLOT_MAP_CACHE_LINE_SIZE
    const size_t combinedLookupSize = initialCapacity * (sizeof(FSDS_SlotMapLookupLinkedListNode) + sizeof(size_t));
    //round combinedLookupSize up to the nearest FSDS_SLOT_MAP_CACHE_LINE_SIZE
    const size_t combinedLookupSizeWithPadding = (combinedLookupSize + FSDS_SLOT_MAP_CACHE_LINE_SIZE - 1) & ~(FSDS_SLOT_MAP_CACHE_LINE_SIZE - 1);

    //round that data up to the FSDS_SLOT_MAP_CACHE_LINE_SIZE so by construction the alocation is a multiple of FSDS_SLOT_MAP_CACHE_LINE_SIZE.
    //This is a requirement of aligned_alloc
    const size_t roundedDataSize = ((initialCapacity * elementSize) + FSDS_SLOT_MAP_CACHE_LINE_SIZE - 1) & ~(FSDS_SLOT_MAP_CACHE_LINE_SIZE - 1);

    //use offsetof instead of sizeof as the c allows sizeof to do funky things and we just want the size of the header and the padding after it before the flexible array member 
    const size_t alocationSize = offsetof(FSDS_SlotMapBlock, data) + combinedLookupSizeWithPadding + roundedDataSize;
    slotMap->memBlock = (FSDS_SlotMapBlock*)aligned_alloc(FSDS_SLOT_MAP_CACHE_LINE_SIZE, alocationSize);
    if(slotMap->memBlock == nullptr) { return FSDS_SLOT_MAP_ERROR_ALOCATION_FAILED; }

    FSDS_SlotMapHeader* header = &slotMap->memBlock->header;
    header->size = 0;
    header->capacity = initialCapacity;
    header->nextLookup = FSDS_SLOT_MAP_NPOS;
    *(size_t*)&header->elementSize = elementSize;
    header->reverseLookupList = (size_t*)(slotMap->memBlock->data + (initialCapacity * sizeof(FSDS_SlotMapLookupLinkedListNode)));
    header->dataList = (void*)(slotMap->memBlock->data + combinedLookupSizeWithPadding);
    return 0;
}
int FSDS_SlotMap_destroy(const FSDS_SlotMap slotMap)
{
    free(slotMap.memBlock);
    return 0;
}

size_t FSDS_SlotMap_size(const FSDS_SlotMap slotMap)
{
    return slotMap.memBlock->header.size;
}
size_t FSDS_SlotMap_capacity(const FSDS_SlotMap slotMap)
{
    return slotMap.memBlock->header.capacity;
}

int FSDS_SlotMap_reserve(FSDS_SlotMap* const slotMap, size_t newCapacity)
{
    if(newCapacity < slotMap->memBlock->header.capacity)
	{
		return FSDS_SLOT_MAP_ERROR_CAPACITY_TOO_SMALL;
	}
    FSDS_RETERR(FSDS_SlotMap_reallocate(slotMap, newCapacity));
    return 0;
}
int FSDS_SlotMap_shrinkToFit(FSDS_SlotMap* const slotMap)
{
    //reallocate enough space for the lookup list
    return 0;
}
int FSDS_SlotMap_clear(const FSDS_SlotMap slotMap)
{
    slotMap.memBlock->header.size = 0;
    slotMap.memBlock->header.nextLookup = FSDS_SLOT_MAP_NPOS;
    return 0;
}

int FSDS_SlotMap_add(FSDS_SlotMap* const slotMap, void** outPtr, FSDS_SlotMapReference* outRef)
{
    if(slotMap->memBlock->header.size >= slotMap->memBlock->header.capacity)
    {
        FSDS_RETERR(FSDS_SlotMap_reallocate(slotMap, slotMap->memBlock->header.capacity * 2));
    }
    
    //find the next lookup
    uint64_t lookupIndex;
    if(slotMap->memBlock->header.nextLookup != FSDS_SLOT_MAP_NPOS)
    {
        lookupIndex = slotMap->memBlock->header.nextLookup;
        //remove the lookup we have chosen from the linked list
        //equivilent of head = head.next
        slotMap->memBlock->header.nextLookup = FSDS_SlotMap_getLookupList(*slotMap)[slotMap->memBlock->header.nextLookup].next & FSDS_SLOT_MAP_INDEX_MASK;
        //the genCount was already incremented when this lookup was freed in FSDS_SlotMap_remove so we simply reuse the current genCount here
    }
    else
    {
        //if the nextLookup is null then every lookup is in use so there are no unused lookups so the lookup list is contiguous.
        //This implies that the last used lookup has the same index as the last used piece of data and the size member can be used to get the next lookup that has not yet been used.
        //As the lookup has not been used before both it and its genCount will be uninitialised so the genCount needs to be set to zero.
        lookupIndex = slotMap->memBlock->header.size;
        //zero the genCount and leave the index unchanged
        FSDS_SlotMap_getLookupList(*slotMap)[lookupIndex].lookup &= ~FSDS_SLOT_MAP_GENCOUNT_MASK;
    }
    //set the lookup's index to be the size
    FSDS_SlotMap_getLookupList(*slotMap)[lookupIndex].lookup = (FSDS_SlotMap_getLookupList(*slotMap)[lookupIndex].lookup & ~FSDS_SLOT_MAP_INDEX_MASK) | (slotMap->memBlock->header.size & FSDS_SLOT_MAP_INDEX_MASK);
    *outPtr = (char*)slotMap->memBlock->header.dataList + (slotMap->memBlock->header.size * slotMap->memBlock->header.elementSize);
    //pack the reference
    *outRef = (lookupIndex & FSDS_SLOT_MAP_INDEX_MASK) | (FSDS_SlotMap_getLookupList(*slotMap)[lookupIndex].lookup & FSDS_SLOT_MAP_GENCOUNT_MASK);
    //add new element to the reverse lookup
    slotMap->memBlock->header.reverseLookupList[slotMap->memBlock->header.size] = lookupIndex;
    slotMap->memBlock->header.size++;
    return 0;
}
int FSDS_SlotMap_remove(const FSDS_SlotMap slotMap, FSDS_SlotMapReference const ref)
{
    const uint64_t lookupIndex = ref & FSDS_SLOT_MAP_INDEX_MASK;
    if(lookupIndex >= slotMap.memBlock->header.capacity)
    {
        return FSDS_SLOT_MAP_ERROR_INDEX_OUT_OF_BOUNDS;
    }
    if((FSDS_SlotMap_getLookupList(slotMap)[lookupIndex].lookup & FSDS_SLOT_MAP_GENCOUNT_MASK) != (ref & FSDS_SLOT_MAP_GENCOUNT_MASK))
    {
        return FSDS_SLOT_MAP_ERROR_INCORRECT_GENERATION;
    }
    const uint64_t dataIndex = FSDS_SlotMap_getLookupList(slotMap)[lookupIndex].lookup & FSDS_SLOT_MAP_INDEX_MASK;
    const size_t elementSize = slotMap.memBlock->header.elementSize;
    const size_t size = slotMap.memBlock->header.size;
    //this lookup is being freed so increment its genCount to invalidate any outstanding references to it.
    //the increment only touches the upper 24 genCount bits so it does not disturb the free-list next pointer or index bits set below.
    //note: this relies on integer overflow which is defined behavior in c
    FSDS_SlotMap_getLookupList(slotMap)[lookupIndex].lookup += FSDS_SLOT_MAP_GENCOUNT_INCREMENT;
    //extend the linked list tracking the next first level list
    FSDS_SlotMap_getLookupList(slotMap)[lookupIndex].next = (slotMap.memBlock->header.nextLookup & FSDS_SLOT_MAP_INDEX_MASK) | (FSDS_SlotMap_getLookupList(slotMap)[lookupIndex].next & FSDS_SLOT_MAP_GENCOUNT_MASK);
    slotMap.memBlock->header.nextLookup = lookupIndex;
    if(dataIndex != size - 1)
    {
        //move the last element of the data to the one being removed
        memcpy((char*)slotMap.memBlock->header.dataList + (dataIndex * elementSize), (char*)slotMap.memBlock->header.dataList + ((size - 1) * elementSize), elementSize);
        //we do not need to move the genCount as neither are being updated
        //update the lookup for the elemnent that was moved forwards
        const size_t reverseIndex = slotMap.memBlock->header.reverseLookupList[size - 1];
        FSDS_SlotMap_getLookupList(slotMap)[reverseIndex].lookup = (FSDS_SlotMap_getLookupList(slotMap)[reverseIndex].lookup & ~FSDS_SLOT_MAP_INDEX_MASK) | (dataIndex & FSDS_SLOT_MAP_INDEX_MASK);
        //move the reverse lookup
        slotMap.memBlock->header.reverseLookupList[dataIndex] = reverseIndex;
    }
    slotMap.memBlock->header.size--;
    return 0;
}
int FSDS_SlotMap_get(const FSDS_SlotMap slotMap, FSDS_SlotMapReference const ref, void** outPtr)
{
    const uint64_t lookupIndex = ref & FSDS_SLOT_MAP_INDEX_MASK;
    if(lookupIndex >= slotMap.memBlock->header.capacity)
    {
        return FSDS_SLOT_MAP_ERROR_INDEX_OUT_OF_BOUNDS;
    }
    if((FSDS_SlotMap_getLookupList(slotMap)[lookupIndex].lookup & FSDS_SLOT_MAP_GENCOUNT_MASK) != (ref & FSDS_SLOT_MAP_GENCOUNT_MASK))
    {
        return FSDS_SLOT_MAP_ERROR_INCORRECT_GENERATION;
    }
    const uint64_t dataIndex = FSDS_SlotMap_getLookupList(slotMap)[lookupIndex].lookup & FSDS_SLOT_MAP_INDEX_MASK;
    *outPtr = (char*)slotMap.memBlock->header.dataList + (dataIndex * slotMap.memBlock->header.elementSize);
    return 0;
}

const char* FSDS_SlotMap_errorString(const int errorCode)
{
	switch(errorCode)
	{
        case FSDS_SLOT_MAP_SUCCESS:
            return "success";
        case FSDS_SLOT_MAP_ERROR_ALOCATION_FAILED:
            return "memory allocation failed";
        case FSDS_SLOT_MAP_ERROR_INVALID_MEMORY_ALIGNMENT:
            return "memory is incorrectly alligned";
        case FSDS_SLOT_MAP_ERROR_CAPACITY_TOO_SMALL:
            return "requested capacity is smaller than current capacity";
        case FSDS_SLOT_MAP_ERROR_INDEX_OUT_OF_BOUNDS:
            return "index is out of bounds (greater than or equal to size)";
        case FSDS_SLOT_MAP_ERROR_INCORRECT_GENERATION:
            return "requested iRef does not match the stored generation (use after remove)";
		default:
			return "unknown FSDS_SlotMap error code";
	}
}