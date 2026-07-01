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

static inline size_t FSDS_genericKTree_calculateNumElements(const size_t k, const uint32_t depth)
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

static inline int FSDS_genericKTree_add(FSDS_KTreeBlock* memBlock, size_t index, void** outPtr)
{
	if(index >= memBlock->header.capacity)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	const size_t i = index / 64;
	const size_t j = index % 64;
	const uint64_t mask = 1ULL << j;
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
	if(index >= memBlock->header.capacity)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	const size_t i = index / 64;
	const size_t j = index % 64;
	const uint64_t mask = 1ULL << j;
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
	if(index >= memBlock->header.capacity)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	const size_t i = index / 64;
	const size_t j = index % 64;
	const uint64_t mask = 1ULL << j;
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
	if(index >= memBlock->header.capacity)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	const size_t i = index / 64;
	const size_t j = index % 64;
	const uint64_t mask = 1ULL << j;
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
	if(index >= memBlock->header.capacity)
	{
		*result = false;
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	const size_t i = index / 64;
	const size_t j = index % 64;
	const uint64_t mask = 1ULL << j;
	const uint64_t isActive = FSDS_genericKTree_getActiveList(memBlock)[i] & mask;
	*result = isActive != 0;
	return FSDS_K_TREE_SUCCESS;
}

static inline void FSDS_genericKTree_copyElement(FSDS_KTreeBlock* oldMemBlock, FSDS_KTreeBlock* newMemBlock, const size_t dstIndex, const size_t srcIndex)
{
	memcpy((char*)newMemBlock->header.dataList + (oldMemBlock->header.elementSize * dstIndex), (char*)oldMemBlock->header.dataList + (oldMemBlock->header.elementSize * srcIndex), oldMemBlock->header.elementSize);
}
//copies the single active bit from srcIndex to dstIndex. Note: newMemBlock's bitmask must be zeroed beforehand
static inline void FSDS_genericKTree_copyActiveBit(FSDS_KTreeBlock* oldMemBlock, FSDS_KTreeBlock* newMemBlock, const size_t dstIndex, const size_t srcIndex)
{
	const uint64_t srcBit = (FSDS_genericKTree_getActiveList(oldMemBlock)[srcIndex / 64] >> (srcIndex % 64)) & 1ULL;
	FSDS_genericKTree_getActiveList(newMemBlock)[dstIndex / 64] |= (srcBit << (dstIndex % 64));
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
	const size_t numElements = FSDS_genericKTree_calculateNumElements(k, depth);
	const size_t numBitmaskInts = FSDS_genericKTree_calculateNumBitmaskInts(numElements);
	
	const size_t roundedUpNumBitmaskIntsBytes = ((numBitmaskInts * sizeof(uint64_t)) + (size_t)FSDS_K_TREE_CACHE_LINE_SIZE - 1ULL) & ~((size_t)FSDS_K_TREE_CACHE_LINE_SIZE - 1ULL);
	const size_t roundedUpElementBytes = ((numElements * elementSize) + (size_t)FSDS_K_TREE_CACHE_LINE_SIZE - 1ULL) & ~((size_t)FSDS_K_TREE_CACHE_LINE_SIZE - 1ULL);

	const size_t elementByteOffset = offsetof(FSDS_KTreeBlock, data) + roundedUpNumBitmaskIntsBytes;
	const size_t allocationSize =  offsetof(FSDS_KTreeBlock, data) + roundedUpNumBitmaskIntsBytes + roundedUpElementBytes;

	tree->memBlock = (FSDS_KTreeBlock*)aligned_alloc(FSDS_K_TREE_CACHE_LINE_SIZE, allocationSize);
	if(tree->memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_ALLOCATION_FAILED;
	}
	//zero the bitmask
	memset(FSDS_genericKTree_getActiveList(tree->memBlock), 0, numBitmaskInts * sizeof(uint64_t));

	FSDS_KTreeHeader* header = &tree->memBlock->header;
	header->size = 0;
	header->capacity = FSDS_genericKTree_calculateNumElements(k, depth);
	header->k = k;
	header->depth = depth;
	header->dataList = (char*)header + elementByteOffset;
	*(size_t*)&header->elementSize = elementSize;

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_BFSKTree_destroy(const FSDS_BFSKTree tree)
{
	if(tree.memBlock != nullptr)
	{
		free(tree.memBlock);
	}

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
	if(newDepth <= tree->memBlock->header.depth)
	{
		return FSDS_K_TREE_ERROR_CAPACITY_TOO_SMALL;
	}

	FSDS_KTreeBlock* oldMemBlock = tree->memBlock;
	FSDS_KTreeHeader* oldHeader = &tree->memBlock->header;
	void* oldData = tree->memBlock->data;

	//in the event of allocation failure restore the tree to it's original state
	int retcode = FSDS_BFSKTree_constructDepth(tree, oldHeader->k, newDepth, oldHeader->elementSize);
	if(retcode != FSDS_K_TREE_SUCCESS)
	{
		tree->memBlock = oldMemBlock;
		return retcode;
	}

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
	const size_t k = tree->memBlock->header.k;
	const size_t elementSize = tree->memBlock->header.elementSize;
	FSDS_RETERR(FSDS_BFSKTree_destroy(*tree));
	FSDS_RETERR(FSDS_BFSKTree_constructDefault(tree, k, elementSize));

	return FSDS_K_TREE_SUCCESS;
}

int FSDS_BFSKTree_calculateIndex(const FSDS_BFSKTree tree, const uint32_t depth, const uint64_t width, size_t* outIndex)
{
	const size_t k = tree.memBlock->header.k;
	size_t index = 0;
	size_t product = 1;
	for(size_t i = 0; i < depth; i++)
	{
		index += product;
		product *= k;
	}
	index += width;
	//perform bounds checking when we know how wide the row could be
	if(width >= product)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	*outIndex = index;
	return FSDS_K_TREE_SUCCESS;
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
	const size_t numElements = FSDS_genericKTree_calculateNumElements(k, depth);
	const size_t numBitmaskInts = FSDS_genericKTree_calculateNumBitmaskInts(numElements);
	
	const size_t roundedUpNumBitmaskIntsBytes = ((numBitmaskInts * sizeof(uint64_t)) + (size_t)FSDS_K_TREE_CACHE_LINE_SIZE - 1ULL) & ~((size_t)FSDS_K_TREE_CACHE_LINE_SIZE - 1ULL);
	const size_t roundedUpElementBytes = ((numElements * elementSize) + (size_t)FSDS_K_TREE_CACHE_LINE_SIZE - 1ULL) & ~((size_t)FSDS_K_TREE_CACHE_LINE_SIZE - 1ULL);

	const size_t elementByteOffset = offsetof(FSDS_KTreeBlock, data) + roundedUpNumBitmaskIntsBytes;
	const size_t allocationSize =  offsetof(FSDS_KTreeBlock, data) + roundedUpNumBitmaskIntsBytes + roundedUpElementBytes;

	tree->memBlock = (FSDS_KTreeBlock*)aligned_alloc(FSDS_K_TREE_CACHE_LINE_SIZE, allocationSize);
	if(tree->memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_ALLOCATION_FAILED;
	}
	//zero the bitmask
	memset(FSDS_genericKTree_getActiveList(tree->memBlock), 0, numBitmaskInts * sizeof(uint64_t));

	FSDS_KTreeHeader* header = &tree->memBlock->header;
	header->size = 0;
	header->capacity = FSDS_genericKTree_calculateNumElements(k, depth);
	header->k = k;
	header->depth = depth;
	header->dataList = (char*)header + elementByteOffset;
	*(size_t*)&header->elementSize = elementSize;

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_DFSKTree_destroy(const FSDS_DFSKTree tree)
{
	if(tree.memBlock != nullptr)
	{
		free(tree.memBlock);
	}

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
	if(newDepth <= tree->memBlock->header.depth)
	{
		return FSDS_K_TREE_ERROR_CAPACITY_TOO_SMALL;
	}

	FSDS_KTreeBlock* oldMemBlock = tree->memBlock;
	FSDS_KTreeHeader* oldHeader = &tree->memBlock->header;

	//in the event of allocation failure restore the tree to it's original state
	int retcode = FSDS_DFSKTree_constructDepth(tree, oldHeader->k, newDepth, oldHeader->elementSize);
	if(retcode != FSDS_K_TREE_SUCCESS)
	{
		tree->memBlock = oldMemBlock;
		return retcode;
	}
	
	//keep track of the depth to know when to skip over leaf nodes
	uint32_t* stack = (uint32_t*)alloca(oldHeader->depth * sizeof(uint32_t));
	uint32_t stackIndex = 0;
	memset(stack, 0, oldHeader->depth * sizeof(uint32_t));
	
	//calculate the difference in dstIndex between former leaf nodes for any change in depth
	//this is just calculating the geometric series
	size_t leafJump = oldHeader->k;
	size_t product = oldHeader->k;
	for(size_t i = 0; i < newDepth - oldHeader->depth - 1; i++)
	{
		product *= oldHeader->k;
		leafJump += product;
	}

	size_t dstIndex = 0;
	for(size_t srcIndex = 0; srcIndex < oldHeader->capacity; srcIndex++)
	{
		//backtrack up the stack when needed
		//stack values go from 1 to k inclusive
		//the stackIndex > 0 guard should never trigger for well formed trees as it is safe by invariant
		while((stackIndex > 0) && (stack[stackIndex] == oldHeader->k))
		{
			stack[stackIndex] = 0;
			stackIndex--;
		}
		//copy the element
		FSDS_genericKTree_copyElement(oldMemBlock, tree->memBlock, dstIndex, srcIndex);
		FSDS_genericKTree_copyActiveBit(oldMemBlock, tree->memBlock, dstIndex, srcIndex);
		dstIndex++;
		stack[stackIndex]++;
		//if we are at a leaf increment dstIndex by k for the jump in indexing
		if(stackIndex == oldHeader->depth - 1)
		{
			dstIndex += leafJump;
		}
		//move down the stack if we are not at a leaf node
		else
		{
			stackIndex++;
		}
	}

	FSDS_KTreeHeader* newHeader = &tree->memBlock->header;
	newHeader->size = oldHeader->size;

	free(oldMemBlock);

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_DFSKTree_clear(FSDS_DFSKTree* const tree)
{
	const size_t k = tree->memBlock->header.k;
	const size_t elementSize = tree->memBlock->header.elementSize;
	FSDS_RETERR(FSDS_DFSKTree_destroy(*tree));
	FSDS_RETERR(FSDS_DFSKTree_constructDefault(tree, k, elementSize));

	return FSDS_K_TREE_SUCCESS;
}

int FSDS_DFSKTree_calculateIndex(const FSDS_DFSKTree tree, const uint32_t depth, const uint64_t width, size_t* outIndex)
{
	size_t index = 0;
	uint64_t w = width;
	size_t product = 1;
	for(uint32_t i = 0; i < depth; i++)
	{
		index += w + 1;
		w /= tree.memBlock->header.k;
		product *= tree.memBlock->header.k;
	}
	//perform bounds checking when we know how wide the row could be
	if(width >= product)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}
	*outIndex = index;
	return FSDS_K_TREE_SUCCESS;
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
			return "index is out of bounds (greater than or equal to capacity)";
		case FSDS_K_TREE_ERROR_INDEX_NOT_ACTIVE:
			return "requested index is not active";
		case FSDS_K_TREE_ERROR_INDEX_ALREADY_ACTIVE:
			return "requested index is already active";
		case FSDS_K_TREE_ERROR_CAPACITY_TOO_SMALL:
			return "requested capacity is smaller than current capacity";
		default:
			return "unknown FSDS_KTree error code";
	}
}