#include "stable_list.h"

static size_t globalInstanceIRefOffset = 0;

static int FSDS_StableList_reallocate(FSDS_StableList* const list, size_t newCapacity, size_t newFront)
{
	FSDS_StableListBlock* oldMemBlock = list->memBlock;
	FSDS_StableListHeader* oldHeader = &oldMemBlock->header;

	//allocate new memory
	list->memBlock = list->memBlock = (FSDS_StableListBlock*)malloc(sizeof(FSDS_StableListBlock) + (newCapacity * oldHeader->elementSize));

	//create the new header
	FSDS_StableListHeader* newHeader = &list->memBlock->header;
	newHeader->size = oldHeader->size;
	newHeader->capacity = newCapacity;
	//if noFront is true than this reallocation will not have space preallocated at the front but does not effect subsequent reallocations
	newHeader->front = newFront;
	//given the front will be "reset" add the front to the iRefOffset to keep track
	newHeader->iRefOffset = oldHeader->iRefOffset + oldHeader->front;
	newHeader->instanceIRefOffsetStart = oldHeader->instanceIRefOffsetStart;
	newHeader->reallocationFrontMargin = oldHeader->reallocationFrontMargin;
	*(size_t*)newHeader->elementSize = oldHeader->elementSize;

	//determine how much of the original data to copy
	const size_t numElementsToCopy = newCapacity < oldHeader->size ? newCapacity : oldHeader->size;
	//copy from front position of the old list to the front position of the new list
	memcpy(list->memBlock->data + (newHeader->front * newHeader->elementSize), oldMemBlock->data + (oldHeader->front * oldHeader->elementSize), numElementsToCopy * oldHeader->elementSize);

	return 0;
}

int FSDS_StableList_constructDefault(FSDS_StableList* const list, const size_t elementSize)
{
	return FSDS_StableList_constructSizeFront(list, FSDS_STABLE_LIST_DEFAULT_CAPACITY, FSDS_STABLE_LIST_DEFAULT_FRONT_MARGIN, elementSize);
}
int FSDS_StableList_constructSize(FSDS_StableList* const list, size_t initialCapacity, const size_t elementSize)
{
	return FSDS_StableList_constructSizeFront(list, initialCapacity, FSDS_STABLE_LIST_DEFAULT_FRONT_MARGIN, elementSize);
}
int FSDS_StableList_constructSizeFront(FSDS_StableList* const list, const size_t initialCapacity, const size_t frontMargin, const size_t elementSize)
{
	list->memBlock = (FSDS_StableListBlock*)malloc(sizeof(FSDS_StableListBlock) + (initialCapacity * elementSize));
	if(list->memBlock == nullptr) { return 1; }

	FSDS_StableListHeader* header = &list->memBlock->header;
	header->size = 0;
	header->capacity = initialCapacity;
	header->front = frontMargin;
	header->iRefOffset = 0;
	header->instanceIRefOffsetStart = globalInstanceIRefOffset;
	header->reallocationFrontMargin = frontMargin;
	*(size_t*)header->elementSize = elementSize;
	globalInstanceIRefOffset += FSDS_STABLE_LIST_INSTANCE_IREF_OFFSET_INCREMENT;
	return 0;
}
int FSDS_StableList_destroy(const FSDS_StableList list)
{
	free(list.memBlock);
	return 0;
}

int FSDS_StableList_getElementIndex(const FSDS_StableList list, const size_t index, void** outPtr)
{
	*outPtr = list.memBlock->data + ((index + list.memBlock->header.front) * list.memBlock->header.elementSize);
	return 0;
}
int FSDS_StableList_getElementIRef(const FSDS_StableList list, const FSDS_StableList_IRef iRef, void** outPtr)
{
	*outPtr = list.memBlock->data + ((iRef - list.memBlock->header.iRefOffset) * list.memBlock->header.elementSize);
	return 0;
}

size_t FSDS_StableList_size(const FSDS_StableList list)
{
	return list.memBlock->header.size;
}
size_t FSDS_StableList_capacity(const FSDS_StableList list)
{
	return list.memBlock->header.capacity;
}

int iRefToIndex(const FSDS_StableList list, const FSDS_StableList_IRef iRef, size_t* outIndex)
{
	*outIndex = list.memBlock->header.iRefOffset - list.memBlock->header.front;
	return 0;
}
int indexToIRef(const FSDS_StableList list, const size_t index, FSDS_StableList_IRef* outIRef)
{
	//check that the index is valid so that the created iRef will also be valid
	if(index >= list.memBlock->header.size)
	{
		return 1;
	}
	*outIRef = index + list.memBlock->header.front;
	return 0;
}

int FSDS_StableList_reserve(FSDS_StableList* const list, size_t newCapacity)
{
	if(newCapacity < list->memBlock->header.size)
	{
		return 1;
	}
	FSDS_RETERR(FSDS_StableList_reallocate(list, newCapacity, list->memBlock->header.reallocationFrontMargin));
	return 0;
}
int FSDS_StableList_shrink_to_fit(FSDS_StableList* const list)
{
	FSDS_RETERR(FSDS_StableList_reallocate(list, list->memBlock->header.size, 0));
	return 0;
}
int FSDS_StableList_clear(const FSDS_StableList list)
{
	list.memBlock->header.size = 0;
	return 0;
}

int FSDS_StableList_appendFront(FSDS_StableList* const list, void** outPtr)
{
	if(list->memBlock->header.front == 0)
	{
		//if the list does not have a front margin forcably allocate a margin of 1 at the front to be immediatly filled otherwise do as normal
		if(list->memBlock->header.reallocationFrontMargin == 0)
		{
			FSDS_RETERR(FSDS_StableList_reallocate(list, list->memBlock->header.capacity + 1, 1));
		}
		else
		{
			FSDS_RETERR(FSDS_StableList_reallocate(list, list->memBlock->header.capacity + list->memBlock->header.reallocationFrontMargin, list->memBlock->header.reallocationFrontMargin));
		}
	}
	list->memBlock->header.front--;
	*outPtr = list->memBlock->data + (list->memBlock->header.front * list->memBlock->header.elementSize);
	list->memBlock->header.size++;
	return 0;
}
int FSDS_StableList_appendBack(FSDS_StableList* const list, void** outPtr)
{
	if(list->memBlock->header.size >= list->memBlock->header.capacity)
	{
		FSDS_RETERR(FSDS_StableList_reallocate(list, list->memBlock->header.capacity * 2, list->memBlock->header.reallocationFrontMargin));
	}
	list->memBlock->header.size++;
	*outPtr = list->memBlock->data + ((list->memBlock->header.front + list->memBlock->header.size) * list->memBlock->header.elementSize);
	return 0;
}
int FSDS_StableList_removeFront(const FSDS_StableList list)
{
	list.memBlock->header.front++;
	list.memBlock->header.size--;
	return 0;
}
int FSDS_StableList_removeBack(const FSDS_StableList list)
{
	list.memBlock->header.size--;
	return 0;
}