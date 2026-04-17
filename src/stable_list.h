#ifndef STABLE_LIST_H_HEADER_GUARD
#define STABLE_LIST_H_HEADER_GUARD

#include<stdint.h>
#include<stdbool.h>
#include<stddef.h>
#include<stdlib.h>
#include<string.h>

#include "fsds_common.h"

#define FSDS_STABLE_LIST_DEFAULT_CAPACITY 16
#define FSDS_STABLE_LIST_DEFAULT_FRONT_MARGIN 16
#define FSDS_STABLE_LIST_INSTANCE_IREF_OFFSET_INCREMENT 0x100000000

#ifdef __cplusplus                                                                                                                                                                                                                                                                       
extern "C" {                                                                                                                                                                                                                                                                             
#endif

typedef size_t FSDS_StableList_IRef;

typedef struct FSDS_StableListHeader
{
	size_t size;
	size_t capacity;
	size_t front;
	//"magic" constant to convert iRefs to indecies
	size_t iRefOffset;
	//"magic" constant to seperate iRef spaces of different instances for detection of applying iRef to wrong instnace
	size_t instanceIRefOffsetStart;
	//how much spare space to put at the front of the list
	size_t reallocationFrontMargin;
	//sizeof(T)
	const size_t elementSize;
} FSDS_StableListHeader;

typedef struct FSDS_StableListBlock
{
	FSDS_StableListHeader header;
	char data[];
} FSDS_StableListBlock;

typedef struct FSDS_StableList
{
	FSDS_StableListBlock* memBlock;
} FSDS_StableList;

int FSDS_StableList_constructDefault(FSDS_StableList* const list, const size_t elementSize);
int FSDS_StableList_constructSize(FSDS_StableList* const list, size_t initialCapacity, const size_t elementSize);
int FSDS_StableList_constructSizeFront(FSDS_StableList* const list, const size_t initialCapacity, const size_t frontMargin, const size_t elementSize);
int FSDS_StableList_destroy(const FSDS_StableList list);

int FSDS_StableList_getElementIndex(const FSDS_StableList list, const size_t index, void** outPtr);
int FSDS_StableList_getElementIRef(const FSDS_StableList list, const FSDS_StableList_IRef iRef, void** outPtr);

size_t FSDS_StableList_size(const FSDS_StableList list);
size_t FSDS_StableList_capacity(const FSDS_StableList list);

int FSDS_StableList_iRefToIndex(const FSDS_StableList list, const FSDS_StableList_IRef iRef, size_t* outIndex);
int FSDS_StableList_indexToIRef(const FSDS_StableList list, const size_t index, FSDS_StableList_IRef* outIRef);

int FSDS_StableList_reserve(FSDS_StableList* const list, size_t newCapacity);
int FSDS_StableList_shrink_to_fit(FSDS_StableList* const list);
int FSDS_StableList_clear(const FSDS_StableList list);

//append functions return pointer to the new memory
int FSDS_StableList_appendFront(FSDS_StableList* const list, void** outPtr);
int FSDS_StableList_appendBack(FSDS_StableList* const list, void** outPtr);
int FSDS_StableList_removeFront(const FSDS_StableList list);
int FSDS_StableList_removeBack(const FSDS_StableList list);

#ifdef __cplusplus                                                                                                                                                                                                                                                                       
}               
#endif
#endif //STABLE_LIST_H_HEADER_GUARD