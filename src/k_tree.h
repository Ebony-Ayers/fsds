#ifndef K_TREE_H_HEADER_GUARD
#define K_TREE_H_HEADER_GUARD

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "fsds_common.h"
#include "fsds_alloca.h"

#define FSDS_K_TREE_CACHE_LINE_SIZE 64

//error codes returned by FSDS_KTree functions
#define FSDS_K_TREE_SUCCESS 0
//memory allocation (malloc) failed
#define FSDS_K_TREE_ERROR_ALLOCATION_FAILED 1
//an index argument was greater than or equal to size
#define FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS 2
//an index refers to an element which has been marked as inactive
#define FSDS_K_TREE_ERROR_INDEX_NOT_ACTIVE 3
//an index refers to an element which has been marked as active
#define FSDS_K_TREE_ERROR_INDEX_ALREADY_ACTIVE 4
//a requested capacity was smaller than the current size
#define FSDS_K_TREE_ERROR_CAPACITY_TOO_SMALL 5

#ifdef __cplusplus                                                                                                                                                                                                                                                                       
extern "C" {                                                                                                                                                                                                                                                                             
#endif

typedef struct FSDS_KTreeHeader
{
	size_t size;
	size_t capacity;
	size_t k;
	uint32_t depth;
	void* dataList;
	//sizeof(T)
	const size_t elementSize;
} FSDS_KTreeHeader;

typedef struct FSDS_KTreeBlock
{
	FSDS_KTreeHeader header;
	alignas(FSDS_K_TREE_CACHE_LINE_SIZE) char data[];
} FSDS_KTreeBlock;

typedef struct FSDS_BFSKTree
{
	FSDS_KTreeBlock* memBlock;
} FSDS_BFSKTree;

typedef struct FSDS_DFSKTree
{
	FSDS_KTreeBlock* memBlock;
} FSDS_DFSKTree;

//================================ BFS ================================

int FSDS_BFSKTree_constructDefault(FSDS_BFSKTree* const tree, const size_t k, const size_t elementSize);
int FSDS_BFSKTree_constructDepth(FSDS_BFSKTree* const tree, const size_t k, const uint32_t depth, const size_t elementSize);
int FSDS_BFSKTree_destroy(const FSDS_BFSKTree tree);

size_t FSDS_BFSKTree_size(const FSDS_BFSKTree tree);
size_t FSDS_BFSKTree_capacity(const FSDS_BFSKTree tree);

int FSDS_BFSKTree_reserveDepth(FSDS_BFSKTree* const tree, uint32_t newDepth);
int FSDS_BFSKTree_clear(FSDS_BFSKTree* const tree);

int FSDS_BFSKTree_calculateIndex(const FSDS_BFSKTree tree, const uint32_t depth, const uint64_t width, size_t* outIndex);
int FSDS_BFSKTree_add(const FSDS_BFSKTree tree, size_t index, void** outPtr);
int FSDS_BFSKTree_addGet(const FSDS_BFSKTree tree, size_t index, void** outPtr);
int FSDS_BFSKTree_remove(const FSDS_BFSKTree tree, size_t index);
int FSDS_BFSKTree_get(const FSDS_BFSKTree tree, size_t index, void** outPtr);
int FSDS_BFSKTree_contains(const FSDS_BFSKTree tree, size_t index, bool* result);

/*
bfs tree indexing
       0   
      / \  
     1   2 
    / \ / \
    3 4 5 6
*/

//================================ DFS ================================

int FSDS_DFSKTree_constructDefault(FSDS_DFSKTree* const tree, const size_t k, const size_t elementSize);
int FSDS_DFSKTree_constructDepth(FSDS_DFSKTree* const tree, const size_t k, const uint32_t depth, const size_t elementSize);
int FSDS_DFSKTree_destroy(const FSDS_DFSKTree tree);

size_t FSDS_DFSKTree_size(const FSDS_DFSKTree tree);
size_t FSDS_DFSKTree_capacity(const FSDS_DFSKTree tree);

int FSDS_DFSKTree_reserveDepth(FSDS_DFSKTree* const tree, uint32_t newDepth);
int FSDS_DFSKTree_clear(FSDS_DFSKTree* const tree);

int FSDS_DFSKTree_calculateIndex(const FSDS_DFSKTree tree, const uint32_t depth, const uint64_t width, size_t* outIndex);
int FSDS_DFSKTree_add(const FSDS_DFSKTree tree, size_t index, void** outPtr);
int FSDS_DFSKTree_addGet(const FSDS_DFSKTree tree, size_t index, void** outPtr);
int FSDS_DFSKTree_remove(const FSDS_DFSKTree tree, size_t index);
int FSDS_DFSKTree_get(const FSDS_DFSKTree tree, size_t index, void** outPtr);
int FSDS_DFSKTree_contains(const FSDS_DFSKTree tree, size_t index, bool* result);

/*
dfs tree indexing
       0   
      / \  
     1   4 
    / \ / \
    2 3 5 6
*/

//returns a statically allocated human-readable description of the given error code. The returned pointer is valid for the lifetime of the program and must not be freed.
const char* FSDS_KTree_errorString(const int errorCode);

#ifdef __cplusplus
}
#endif
#endif //K_TREE_H_HEADER_GUARD