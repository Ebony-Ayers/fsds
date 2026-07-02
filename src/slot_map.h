#ifndef SLOT_MAP_H_HEADER_GUARD
#define SLOT_MAP_H_HEADER_GUARD

#include<stdint.h>
#include<stdbool.h>
#include<stddef.h>
#include<stdlib.h>
#include<string.h>

#include "fsds_common.h"

#define FSDS_SLOT_MAP_DEFAULT_CAPACITY 16
#define FSDS_SLOT_MAP_CACHE_LINE_SIZE 64

//error codes returned by FSDS_SlotMap functions
#define FSDS_SLOT_MAP_SUCCESS 0
//memory allocation (malloc) failed
#define FSDS_SLOT_MAP_ERROR_ALOCATION_FAILED 1
//memory is not alligned properly
#define FSDS_SLOT_MAP_ERROR_INVALID_MEMORY_ALIGNMENT 2
//a requested capacity was smaller than the current size
#define FSDS_SLOT_MAP_ERROR_CAPACITY_TOO_SMALL 3
//an index argument was greater than or equal to size
#define FSDS_SLOT_MAP_ERROR_INDEX_OUT_OF_BOUNDS 4
//a reference was used that has the wrong generation
#define FSDS_SLOT_MAP_ERROR_INCORRECT_GENERATION 5

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t FSDS_SlotMapReference;

//strictly this is redundant however it makes it clearer how the data is being used
typedef union FSDS_SlotMapLookupLinkedListNode
{
	uint64_t lookup;
	uint64_t next;
} FSDS_SlotMapLookupLinkedListNode;

typedef struct FSDS_SlotMapHeader
{
    size_t size;
	size_t capacity;
	uint64_t nextLookup;
	const size_t elementSize;
	//will always be 8 byte aligned
	size_t* reverseLookupList;
	//will always be cache line aligned
	void* dataList;
} FSDS_SlotMapHeader;

typedef struct FSDS_SlotMapBlock
{
	FSDS_SlotMapHeader header;
	alignas(FSDS_SLOT_MAP_CACHE_LINE_SIZE) char data[];
} FSDS_SlotMapBlock;

typedef struct FSDS_SlotMap
{
	FSDS_SlotMapBlock* memBlock;
} FSDS_SlotMap;

int FSDS_SlotMap_constructDefault(FSDS_SlotMap* const slotMap, const size_t elementSize);
int FSDS_SlotMap_constructSize(FSDS_SlotMap* const slotMap, size_t initialCapacity, const size_t elementSize);
int FSDS_SlotMap_destroy(const FSDS_SlotMap slotMap);

size_t FSDS_SlotMap_size(const FSDS_SlotMap slotMap);
size_t FSDS_SlotMap_capacity(const FSDS_SlotMap slotMap);

int FSDS_SlotMap_reserve(FSDS_SlotMap* const slotMap, size_t newCapacity);
int FSDS_SlotMap_shrinkToFit(FSDS_SlotMap* const slotMap);
int FSDS_SlotMap_clear(const FSDS_SlotMap slotMap);

int FSDS_SlotMap_add(FSDS_SlotMap* const slotMap, void** outPtr, FSDS_SlotMapReference* outRef);
int FSDS_SlotMap_remove(const FSDS_SlotMap slotMap, FSDS_SlotMapReference const ref);
int FSDS_SlotMap_get(const FSDS_SlotMap slotMap, FSDS_SlotMapReference const ref, void** outPtr);

const char* FSDS_SlotMap_errorString(const int errorCode);

#ifdef __cplusplus
}
#endif
#endif //SLOT_MAP_H_HEADER_GUARD