#include "k_tree.h"

static inline uint64_t* FSDS_genericKTree_getActiveList(FSDS_KTreeBlock* memBlock)
{
	return (uint64_t*)memBlock->data;
}

static inline size_t FSDS_genericKTree_getSize(FSDS_KTreeBlock* memBlock)
{
	return memBlock->header.size;
}
static inline size_t FSDS_genericKTree_getCapacity(FSDS_KTreeBlock* memBlock)
{
	return memBlock->header.capacity;
}

static inline size_t FSDS_genericKTRee_calculateNumElements(const size_t k, const uint32_t depth)
{
	size_t total = 0;
	size_t power = 1;
	for(size_t i = 0; i < depth; i++)
	{
		total += power;
		power *= k;
	}
	return total;
}
static inline size_t FSDS_genericKTree_calculateNumBitmaskInts(const size_t numElements)
{
	return (numElements + 63) / 64;
}
static inline size_t FSDS_genericKTree_calculateAllocationSize(const size_t k, const uint32_t depth, const size_t elementSize)
{
	const size_t numElements = FSDS_genericKTRee_calculateNumElements(k, depth);
	const size_t numBitmaskInts = FSDS_genericKTree_calculateNumBitmaskInts(numElements);
	return offsetof(FSDS_KTreeBlock, data) + (numBitmaskInts * sizeof(uint64_t)) + (numElements * elementSize);
}

static inline int FSDS_genericKTree_add(FSDS_KTreeBlock* memBlock, size_t index, void** outPtr)
{
	if(index >= memBlock->header.size)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	const size_t i = index / 64;
	const size_t j = index % 64;
	const uint64_t mask = 1UL << j;
	const uint64_t isActive = FSDS_genericKTree_getActiveList(memBlock)[i] & mask;
	if(isActive != 0)
	{
		return FSDS_K_TREE_ERROR_INDEX_ALREADY_ACTIVE;
	}
	FSDS_genericKTree_getActiveList(memBlock)[i] |= mask;
	*outPtr = (char*)memBlock->header.dataList + (index * memBlock->header.elementSize);
	memBlock->header.size++;
	return FSDS_K_TREE_SUCCESS;
}
static inline int FSDS_genericKTree_addGet(FSDS_KTreeBlock* memBlock, size_t index, void** outPtr)
{
	if(index >= memBlock->header.size)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	const size_t i = index / 64;
	const size_t j = index % 64;
	const uint64_t mask = 1UL << j;
	const uint64_t isActive = FSDS_genericKTree_getActiveList(memBlock)[i] & mask;
	if(isActive == 0)
	{
		FSDS_genericKTree_getActiveList(memBlock)[i] |= mask;
		memBlock->header.size++;
	}
	*outPtr = (char*)memBlock->header.dataList + (index * memBlock->header.elementSize);
	return FSDS_K_TREE_SUCCESS;
}
static inline int FSDS_genericKTree_remove(FSDS_KTreeBlock* memBlock, size_t index)
{
	if(index >= memBlock->header.size)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	const size_t i = index / 64;
	const size_t j = index % 64;
	const uint64_t mask = 1UL << j;
	const uint64_t isActive = FSDS_genericKTree_getActiveList(memBlock)[i] & mask;
	if(isActive == 0)
	{
		return FSDS_K_TREE_ERROR_INDEX_NOT_ACTIVE;
	}
	FSDS_genericKTree_getActiveList(memBlock)[i] &= ~mask;
	memBlock->header.size--;
	return FSDS_K_TREE_SUCCESS;
}
static inline int FSDS_genericKTree_get(FSDS_KTreeBlock* memBlock, size_t index, void** outPtr)
{
	if(index >= memBlock->header.size)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	const size_t i = index / 64;
	const size_t j = index % 64;
	const uint64_t mask = 1UL << j;
	const uint64_t isActive = FSDS_genericKTree_getActiveList(memBlock)[i] & mask;
	if(isActive == 0)
	{
		return FSDS_K_TREE_ERROR_INDEX_NOT_ACTIVE;
	}
	*outPtr = (char*)memBlock->header.dataList + (index * memBlock->header.elementSize);
	return FSDS_K_TREE_SUCCESS;
}
static inline int FSDS_genericKTree_contains(FSDS_KTreeBlock* memBlock, size_t index, bool* result)
{
	if(index >= memBlock->header.size)
	{
		*result = false;
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	const size_t i = index / 64;
	const size_t j = index % 64;
	const uint64_t mask = 1UL << j;
	const uint64_t isActive = FSDS_genericKTree_getActiveList(memBlock)[i] & mask;
	*result = isActive != 0;
	return FSDS_K_TREE_SUCCESS;
}

static inline void FSDS_genericKTree_copyElement(FSDS_KTreeBlock* oldMemBlock, FSDS_KTreeBlock* newMemBlock, const size_t dstIndex, const size_t srcIndex)
{
	memcpy((char*)newMemBlock->header.dataList + (oldMemBlock->header.elementSize * dstIndex), (char*)oldMemBlock->header.dataList + (oldMemBlock->header.elementSize * srcIndex), oldMemBlock->header.elementSize);
}

//================================ BFS ================================

int FSDS_BFSKTree_constructDefault(FSDS_BFSKTree* const tree, const size_t k, const size_t elementSize)
{
	const uint32_t depth = 1;
	FSDS_RETERR(FSDS_BFSKTree_constructDepth(tree, k, depth, elementSize));
	
	return FSDS_K_TREE_SUCCESS;
}
int FSDS_BFSKTree_constructDepth(FSDS_BFSKTree* const tree, const size_t k, const uint32_t depth, const size_t elementSize)
{
	const size_t allocationSize = FSDS_genericKTree_calculateAllocationSize(k, depth, elementSize);
	tree->memBlock = (FSDS_KTreeBlock*)aligned_alloc(FSDS_K_TREE_CACHE_LINE_SIZE, allocationSize);

	FSDS_KTreeHeader* header = &tree->memBlock->header;
	header->size = 0;
	header->capacity = FSDS_genericKTRee_calculateNumElements(k, depth);
	header->k = k;
	header->depth = depth;
	header->dataList = nullptr; //calculate this
	*(size_t*)&header->elementSize = elementSize;

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_BFSKTree_destroy(const FSDS_BFSKTree tree)
{
	free(tree.memBlock);

	return FSDS_K_TREE_SUCCESS;
}

size_t FSDS_BFSKTree_size(const FSDS_BFSKTree tree)
{
	return FSDS_genericKTree_getSize(tree.memBlock);
}
size_t FSDS_BFSKTree_capacity(const FSDS_BFSKTree tree)
{
	return FSDS_genericKTree_getCapacity(tree.memBlock);
}

int FSDS_BFSKTree_reserveDepth(FSDS_BFSKTree* const tree, uint32_t newDepth)
{
	FSDS_KTreeBlock* oldMemBlock = tree->memBlock;
	FSDS_KTreeHeader* oldHeader = &tree->memBlock->header;
	void* oldData = tree->memBlock->data;

	FSDS_RETERR(FSDS_BFSKTree_constructDepth(tree, oldHeader->k, newDepth, oldHeader->elementSize));

	const size_t numBitmaskInts = FSDS_genericKTree_calculateNumBitmaskInts(oldHeader->capacity);
	memcpy(tree->memBlock->data, oldData, numBitmaskInts * sizeof(uint64_t));
	memcpy(tree->memBlock->header.dataList, oldHeader->dataList, oldHeader->capacity * oldHeader->elementSize);

	FSDS_KTreeHeader* newHeader = &tree->memBlock->header;
	newHeader->size = oldHeader->size;

	free(oldMemBlock);

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_BFSKTree_clear(FSDS_BFSKTree* const tree)
{
	FSDS_RETERR(FSDS_BFSKTree_destroy(*tree));
	FSDS_RETERR(FSDS_BFSKTree_constructDefault(tree, tree->memBlock->header.k, tree->memBlock->header.elementSize));

	return FSDS_K_TREE_SUCCESS;
}

size_t FSDS_BFSKTree_calculateIndex(const FSDS_BFSKTree tree, const uint32_t depth, const uint64_t width)
{
	const size_t k = tree.memBlock->header.k;
	size_t index = 0;
	size_t product = 1;
	for(size_t i = 0; i < depth - 1; i++)
	{
		index += product;
		product *= k;
	}
	index += width;
	return index;
}
int FSDS_BFSKTree_add(const FSDS_BFSKTree tree, size_t index, void** outPtr)
{
	return FSDS_genericKTree_add(tree.memBlock, index, outPtr);
}
int FSDS_BFSKTree_addGet(const FSDS_BFSKTree tree, size_t index, void** outPtr)
{
	return FSDS_genericKTree_addGet(tree.memBlock, index, outPtr);
}
int FSDS_BFSKTree_remove(const FSDS_BFSKTree tree, size_t index)
{
	return FSDS_genericKTree_remove(tree.memBlock, index);
}
int FSDS_BFSKTree_get(const FSDS_BFSKTree tree, size_t index, void** outPtr)
{
	return FSDS_genericKTree_get(tree.memBlock, index, outPtr);
}
int FSDS_BFSKTree_contains(const FSDS_BFSKTree tree, size_t index, bool* result)
{
	return FSDS_genericKTree_contains(tree.memBlock, index, result);
}

//================================ DFS ================================

int FSDS_DFSKTree_constructDefault(FSDS_DFSKTree* const tree, const size_t k, const size_t elementSize)
{
	const uint32_t depth = 1;
	FSDS_RETERR(FSDS_DFSKTree_constructDepth(tree, k, depth, elementSize));
	
	return FSDS_K_TREE_SUCCESS;
}
int FSDS_DFSKTree_constructDepth(FSDS_DFSKTree* const tree, const size_t k, const uint32_t depth, const size_t elementSize)
{
	const size_t allocationSize = FSDS_genericKTree_calculateAllocationSize(k, depth, elementSize);
	tree->memBlock = (FSDS_KTreeBlock*)aligned_alloc(FSDS_K_TREE_CACHE_LINE_SIZE, allocationSize);

	FSDS_KTreeHeader* header = &tree->memBlock->header;
	header->size = 0;
	header->capacity = FSDS_genericKTRee_calculateNumElements(k, depth);
	header->k = k;
	header->depth = depth;
	header->dataList = nullptr; //calculate this
	*(size_t*)&header->elementSize = elementSize;

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_DFSKTree_destroy(const FSDS_DFSKTree tree)
{
	free(tree.memBlock);

	return FSDS_K_TREE_SUCCESS;
}

size_t FSDS_DFSKTree_size(const FSDS_DFSKTree tree)
{
	return FSDS_genericKTree_getSize(tree.memBlock);
}
size_t FSDS_DFSKTree_capacity(const FSDS_DFSKTree tree)
{
	return FSDS_genericKTree_getCapacity(tree.memBlock);
}

int FSDS_DFSKTree_reserveDepth(FSDS_DFSKTree* const tree, uint32_t newDepth)
{
	FSDS_KTreeBlock* oldMemBlock = tree->memBlock;
	FSDS_KTreeHeader* oldHeader = &tree->memBlock->header;
	void* oldData = tree->memBlock->data;

	FSDS_RETERR(FSDS_DFSKTree_constructDepth(tree, oldHeader->k, newDepth, oldHeader->elementSize));

	const size_t numBitmaskInts = FSDS_genericKTree_calculateNumBitmaskInts(oldHeader->capacity);
	memcpy(tree->memBlock->data, oldData, numBitmaskInts * sizeof(uint64_t));
	
	//keep track of the depth to know when to skip over leaf nodes
	uint32_t* stack = (uint32_t*)alloca(oldHeader->depth);
	uint32_t stackIndex = 0;
	memset(stack, 0, oldHeader->depth * sizeof(uint32_t));
	
	size_t dstIndex = 0;
	for(size_t srcIndex = 0; srcIndex < oldHeader->capacity; srcIndex++)
	{
		//backtrack up the stack when needed
		//stack values go from 1 to k inclusive
		while(stack[stackIndex] == oldHeader->k)
		{
			stack[stackIndex] = 0;
			stackIndex--;
		}
		//copy the element
		FSDS_genericKTree_copyElement(oldMemBlock, tree->memBlock, dstIndex, srcIndex);
		dstIndex++;
		stackIndex++;
		//if we are at a leaf increment dstIndex by k for the jump in indexing
		if(stackIndex == oldHeader->depth - 1)
		{
			dstIndex += oldHeader->k;
		}
		//move down the stack if we are not at a leaf node
		else
		{
			stack[stackIndex]++;
		}
	}

	FSDS_KTreeHeader* newHeader = &tree->memBlock->header;
	newHeader->size = oldHeader->size;

	free(oldMemBlock);

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_DFSKTree_clear(FSDS_DFSKTree* const tree)
{
	FSDS_RETERR(FSDS_DFSKTree_destroy(*tree));
	FSDS_RETERR(FSDS_DFSKTree_constructDefault(tree, tree->memBlock->header.k, tree->memBlock->header.elementSize));

	return FSDS_K_TREE_SUCCESS;
}

size_t FSDS_DFSKTree_calculateIndex(const FSDS_DFSKTree tree, const uint32_t depth, const uint64_t width)
{
	size_t index = 0;
	uint64_t w = width;	
	for(uint32_t i = 0; i < depth; i++)
	{
		index += w + 1;
		w /= tree.memBlock->header.k;
	}
	return index;
}
int FSDS_DFSKTree_add(const FSDS_DFSKTree tree, size_t index, void** outPtr)
{
	return FSDS_genericKTree_add(tree.memBlock, index, outPtr);
}
int FSDS_DFSKTree_addGet(const FSDS_DFSKTree tree, size_t index, void** outPtr)
{
	return FSDS_genericKTree_addGet(tree.memBlock, index, outPtr);
}
int FSDS_DFSKTree_remove(const FSDS_DFSKTree tree, size_t index)
{
	return FSDS_genericKTree_remove(tree.memBlock, index);
}
int FSDS_DFSKTree_get(const FSDS_DFSKTree tree, size_t index, void** outPtr)
{
	return FSDS_genericKTree_get(tree.memBlock, index, outPtr);
}
int FSDS_DFSKTree_contains(const FSDS_DFSKTree tree, size_t index, bool* result)
{
	return FSDS_genericKTree_contains(tree.memBlock, index, result);
}



const char* FSDS_KTree_errorString(const int errorCode)
{
	switch(errorCode)
	{
		case FSDS_K_TREE_SUCCESS:
			return "success";
		case FSDS_K_TREE_ERROR_ALLOCATION_FAILED:
			return "memory allocation failed";
		case FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS:
			return "index is out of bounds (greater than or equal to size)";
		case FSDS_K_TREE_ERROR_INDEX_NOT_ACTIVE:
			return "requested index is not active";
		case FSDS_K_TREE_ERROR_INDEX_ALREADY_ACTIVE:
			return "requested index is already active";
		case FSDS_K_TREE_ERROR_CAPACITY_TOO_SMALL:
			return "requested capacity is smaller than current size";
		default:
			return "unknown FSDS_KTree error code";
	}
}