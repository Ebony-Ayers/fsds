#include "k_tree.h"

#include <string.h>
#include <stdlib.h>
#include <stdckdint.h>

#include "../fsds_alloca.h"

//note: every bit at index >= capacity must always be zero
static inline uint64_t* FSDS_genericKTree_getActiveList(FSDS_KTreeBlock* memBlock)
{
	return (uint64_t*)memBlock->data;
}

static inline int FSDS_genericKTree_getSize(FSDS_KTreeBlock* memBlock, size_t* outSize)
{
	if(memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(outSize == nullptr)
	{
		return FSDS_K_TREE_ERROR_OUT_NULL_PTR;
	}

	*outSize = memBlock->header.size;
	return FSDS_K_TREE_SUCCESS;
}
static inline int FSDS_genericKTree_getCapacity(FSDS_KTreeBlock* memBlock, size_t* outCapacity)
{
	if(memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(outCapacity == nullptr)
	{
		return FSDS_K_TREE_ERROR_OUT_NULL_PTR;
	}

	*outCapacity = memBlock->header.capacity;
	return FSDS_K_TREE_SUCCESS;
}

//calculates the number of elements in a tree of the given dimensions, returning FSDS_K_TREE_ERROR_PARAMS_INT_OVERFLOW if the geometric series overflows size_t
static inline int FSDS_genericKTree_calculateNumElements(const size_t k, const uint32_t depth, size_t* outNumElements)
{
	size_t total = 0;
	size_t power = 1;
	for(size_t i = 0; i < depth; i++)
	{
		if(ckd_add(&total, total, power))
		{
			return FSDS_K_TREE_ERROR_PARAMS_INT_OVERFLOW;
		}
		//the multiply on the final iteration is unused, so skip it to avoid a spurious overflow
		if((i + 1 < depth) && ckd_mul(&power, power, k))
		{
			return FSDS_K_TREE_ERROR_PARAMS_INT_OVERFLOW;
		}
	}
	*outNumElements = total;
	return FSDS_K_TREE_SUCCESS;
}
static inline size_t FSDS_genericKTree_calculateNumBitmaskInts(const size_t numElements)
{
	return (numElements + 63) / 64;
}

static inline int FSDS_genericKTree_add(FSDS_KTreeBlock* memBlock, size_t index, void** outPtr)
{
	if(memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(outPtr == nullptr)
	{
		return FSDS_K_TREE_ERROR_OUT_NULL_PTR;
	}
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
	if(memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(outPtr == nullptr)
	{
		return FSDS_K_TREE_ERROR_OUT_NULL_PTR;
	}
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
	if(memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
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
	if(memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(outPtr == nullptr)
	{
		return FSDS_K_TREE_ERROR_OUT_NULL_PTR;
	}
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
	if(memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(result == nullptr)
	{
		return FSDS_K_TREE_ERROR_OUT_NULL_PTR;
	}
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
	if(k < 2)
	{
		return FSDS_K_TREE_ERROR_K_TOO_SMALL;
	}
	if(depth == 0)
	{
		return FSDS_K_TREE_ERROR_DEPTH_ZERO;
	}
	if(elementSize == 0)
	{
		return FSDS_K_TREE_ERROR_ELEM_SIZE_TOO_SMALL;
	}

	size_t numElements;
	FSDS_RETERR(FSDS_genericKTree_calculateNumElements(k, depth, &numElements));
	const size_t numBitmaskInts = FSDS_genericKTree_calculateNumBitmaskInts(numElements);
	
	const size_t cacheMask = (size_t)FSDS_K_TREE_CACHE_LINE_SIZE - 1ULL;
	size_t roundedUpNumBitmaskIntsBytes;
	if(ckd_mul(&roundedUpNumBitmaskIntsBytes, numBitmaskInts, sizeof(uint64_t)) || ckd_add(&roundedUpNumBitmaskIntsBytes, roundedUpNumBitmaskIntsBytes, cacheMask))
	{
		return FSDS_K_TREE_ERROR_PARAMS_INT_OVERFLOW;
	}
	roundedUpNumBitmaskIntsBytes &= ~cacheMask;
	size_t roundedUpElementBytes;
	if(ckd_mul(&roundedUpElementBytes, numElements, elementSize) || ckd_add(&roundedUpElementBytes, roundedUpElementBytes, cacheMask))
	{
		return FSDS_K_TREE_ERROR_PARAMS_INT_OVERFLOW;
	}
	roundedUpElementBytes &= ~cacheMask;

	size_t elementByteOffset;
	size_t allocationSize;
	if(ckd_add(&elementByteOffset, offsetof(FSDS_KTreeBlock, data), roundedUpNumBitmaskIntsBytes) || ckd_add(&allocationSize, elementByteOffset, roundedUpElementBytes))
	{
		return FSDS_K_TREE_ERROR_PARAMS_INT_OVERFLOW;
	}

	tree->memBlock = (FSDS_KTreeBlock*)aligned_alloc(FSDS_K_TREE_CACHE_LINE_SIZE, allocationSize);
	if(tree->memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_ALLOCATION_FAILED;
	}
	//zero the bitmask
	memset(FSDS_genericKTree_getActiveList(tree->memBlock), 0, numBitmaskInts * sizeof(uint64_t));

	FSDS_KTreeHeader* header = &tree->memBlock->header;
	header->size = 0;
	header->capacity = numElements;
	header->k = k;
	header->depth = depth;
	header->dataList = (char*)header + elementByteOffset;
	*(size_t*)&header->elementSize = elementSize;

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_BFSKTree_destroy(FSDS_BFSKTree* const tree)
{
	if(tree->memBlock != nullptr)
	{
		free(tree->memBlock);
		tree->memBlock = nullptr;
	}

	return FSDS_K_TREE_SUCCESS;
}

int FSDS_BFSKTree_size(const FSDS_BFSKTree tree, size_t* outSize)
{
	return FSDS_genericKTree_getSize(tree.memBlock, outSize);
}
int FSDS_BFSKTree_capacity(const FSDS_BFSKTree tree, size_t* outCapacity)
{
	return FSDS_genericKTree_getCapacity(tree.memBlock, outCapacity);
}

int FSDS_BFSKTree_increaseDepth(FSDS_BFSKTree* const tree, uint32_t newDepth)
{
	if(tree->memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(newDepth <= tree->memBlock->header.depth)
	{
		return FSDS_K_TREE_ERROR_DEPTH_TOO_SMALL;
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
int FSDS_BFSKTree_decreaseDepth(FSDS_BFSKTree* const tree, uint32_t newDepth)
{
	if(tree->memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(newDepth >= tree->memBlock->header.depth)
	{
		return FSDS_K_TREE_ERROR_DEPTH_TOO_BIG;
	}
	if(newDepth == 0)
	{
		return FSDS_K_TREE_ERROR_DEPTH_ZERO;
	}

	size_t newCapacity = 0;
	size_t pow = 1;
	for(uint32_t i = 0; i < newDepth; i++)
	{
		newCapacity += pow;
		pow *= tree->memBlock->header.k;
	}
	size_t newSize = 0;
	for(size_t i = 0; i < newCapacity; i++)
	{
		const size_t j = i / 64;
		const size_t k = i % 64;
		const uint64_t mask = 1ULL << k;
		const uint64_t isActive = FSDS_genericKTree_getActiveList(tree->memBlock)[j] & mask;
		newSize += isActive > 0;
	}
	//zero any remaining bits as increaseDepth requires it
	for(size_t i = newCapacity; i < tree->memBlock->header.capacity; i++)
	{
		const size_t j = i / 64;
		const size_t k = i % 64;
		const uint64_t mask = ~(1ULL << k);
		FSDS_genericKTree_getActiveList(tree->memBlock)[j] &= mask;
	}
	tree->memBlock->header.capacity = newCapacity;
	tree->memBlock->header.size = newSize;
	tree->memBlock->header.depth = newDepth;

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_BFSKTree_clear(FSDS_BFSKTree* const tree)
{
	if(tree->memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}

	const size_t k = tree->memBlock->header.k;
	const size_t elementSize = tree->memBlock->header.elementSize;
	FSDS_RETERR(FSDS_BFSKTree_destroy(tree));
	FSDS_RETERR(FSDS_BFSKTree_constructDefault(tree, k, elementSize));

	return FSDS_K_TREE_SUCCESS;
}

int FSDS_BFSKTree_calculateIndex(const FSDS_BFSKTree tree, const uint32_t depth, const uint64_t width, size_t* outIndex)
{
	if(tree.memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(outIndex == nullptr)
	{
		return FSDS_K_TREE_ERROR_OUT_NULL_PTR;
	}
	if(depth >= tree.memBlock->header.depth)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}

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
	if(k < 2)
	{
		return FSDS_K_TREE_ERROR_K_TOO_SMALL;
	}
	if(depth == 0)
	{
		return FSDS_K_TREE_ERROR_DEPTH_ZERO;
	}
	if(elementSize == 0)
	{
		return FSDS_K_TREE_ERROR_ELEM_SIZE_TOO_SMALL;
	}

	size_t numElements;
	FSDS_RETERR(FSDS_genericKTree_calculateNumElements(k, depth, &numElements));
	const size_t numBitmaskInts = FSDS_genericKTree_calculateNumBitmaskInts(numElements);
	
	const size_t cacheMask = (size_t)FSDS_K_TREE_CACHE_LINE_SIZE - 1ULL;
	size_t roundedUpNumBitmaskIntsBytes;
	if(ckd_mul(&roundedUpNumBitmaskIntsBytes, numBitmaskInts, sizeof(uint64_t)) || ckd_add(&roundedUpNumBitmaskIntsBytes, roundedUpNumBitmaskIntsBytes, cacheMask))
	{
		return FSDS_K_TREE_ERROR_PARAMS_INT_OVERFLOW;
	}
	roundedUpNumBitmaskIntsBytes &= ~cacheMask;
	size_t roundedUpElementBytes;
	if(ckd_mul(&roundedUpElementBytes, numElements, elementSize) || ckd_add(&roundedUpElementBytes, roundedUpElementBytes, cacheMask))
	{
		return FSDS_K_TREE_ERROR_PARAMS_INT_OVERFLOW;
	}
	roundedUpElementBytes &= ~cacheMask;

	size_t elementByteOffset;
	size_t allocationSize;
	if(ckd_add(&elementByteOffset, offsetof(FSDS_KTreeBlock, data), roundedUpNumBitmaskIntsBytes) || ckd_add(&allocationSize, elementByteOffset, roundedUpElementBytes))
	{
		return FSDS_K_TREE_ERROR_PARAMS_INT_OVERFLOW;
	}

	tree->memBlock = (FSDS_KTreeBlock*)aligned_alloc(FSDS_K_TREE_CACHE_LINE_SIZE, allocationSize);
	if(tree->memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_ALLOCATION_FAILED;
	}
	//zero the bitmask
	memset(FSDS_genericKTree_getActiveList(tree->memBlock), 0, numBitmaskInts * sizeof(uint64_t));

	FSDS_KTreeHeader* header = &tree->memBlock->header;
	header->size = 0;
	header->capacity = numElements;
	header->k = k;
	header->depth = depth;
	header->dataList = (char*)header + elementByteOffset;
	*(size_t*)&header->elementSize = elementSize;

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_DFSKTree_destroy(FSDS_DFSKTree* const tree)
{
	if(tree->memBlock != nullptr)
	{
		free(tree->memBlock);
		tree->memBlock = nullptr;
	}

	return FSDS_K_TREE_SUCCESS;
}

int FSDS_DFSKTree_size(const FSDS_DFSKTree tree, size_t* outSize)
{
	return FSDS_genericKTree_getSize(tree.memBlock, outSize);
}
int FSDS_DFSKTree_capacity(const FSDS_DFSKTree tree, size_t* outCapacity)
{
	return FSDS_genericKTree_getCapacity(tree.memBlock, outCapacity);
}

int FSDS_DFSKTree_increaseDepth(FSDS_DFSKTree* const tree, uint32_t newDepth)
{
	if(tree->memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(newDepth <= tree->memBlock->header.depth)
	{
		return FSDS_K_TREE_ERROR_DEPTH_TOO_SMALL;
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
	
	//to avoid resursive functions create a stack to track the walk
	//we need to know weather we are copying a leaf or non-leaf node and tracks that
	//each value on the stack goes from 1 to k inclusive, counting which child we are up to at least depth
	//note: this is not an index
	uint32_t* stack = (uint32_t*)alloca(oldHeader->depth * sizeof(uint32_t));
	uint32_t stackIndex = 0;
	memset(stack, 0, oldHeader->depth * sizeof(uint32_t));
	
	//leafJump is the size of the subtree being added to each leaf. This is just the geoemtric series for the number of new depths being added
	size_t leafJump = oldHeader->k;
	size_t product = oldHeader->k;
	for(size_t i = 0; i < newDepth - oldHeader->depth - 1; i++)
	{
		product *= oldHeader->k;
		leafJump += product;
	}

	//copy the elements one at a time contiguously from src but discontiguously into dst
	//discontiguities occur when we add a subtree as tracked by the stack
	size_t dstIndex = 0;
	for(size_t srcIndex = 0; srcIndex < oldHeader->capacity; srcIndex++)
	{
		//if we have copied every child for a given depth backtrack up the stack
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
		//increment the child counter on the stack
		stack[stackIndex]++;
		//if we are at a leaf increment dstIndex by leafJump as this is where we are adding a subtree
		if(stackIndex == oldHeader->depth - 1)
		{
			dstIndex += leafJump;
		}
		//if we are not at a leaf node move down the stack
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
int FSDS_DFSKTree_decreaseDepth(FSDS_DFSKTree* const tree, uint32_t newDepth)
{
	if(tree->memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(newDepth >= tree->memBlock->header.depth)
	{
		return FSDS_K_TREE_ERROR_DEPTH_TOO_BIG;
	}
	if(newDepth == 0)
	{
		return FSDS_K_TREE_ERROR_DEPTH_ZERO;
	}

	FSDS_KTreeBlock* memBlock = tree->memBlock;
	FSDS_KTreeHeader* header = &memBlock->header;

	size_t newCapacity = 0;
	size_t pow = 1;
	for(uint32_t i = 0; i < newDepth; i++)
	{
		newCapacity += pow;
		pow *= header->k;
	}

	//to avoid resursive functions create a stack to track the walk
	//we need to know weather we are copying a leaf or non-leaf node and tracks that
	//each value on the stack goes from 1 to k inclusive, counting which child we are up to at least depth
	//note: this is not an index
	uint32_t* stack = (uint32_t*)alloca(newDepth * sizeof(uint32_t));
	uint32_t stackIndex = 0;
	memset(stack, 0, newDepth * sizeof(uint32_t));

	//srcJump is the size of the subtree being removed from each node that will become leaf. This is just the geoemtric series for the number of new depths being added
	size_t srcJump = header->k;
	size_t product = header->k;
	for(size_t i = 0; i < header->depth - newDepth - 1; i++)
	{
		product *= header->k;
		srcJump += product;
	}

	//copy the elements one at a time discontiguously from src but contiguously into dst
	//discontiguities occur when we remove a subtree as tracked by the stack
	//in effect we are shuffeling elements towards the front of the array
	size_t newSize = 0;
	size_t srcIndex = 0;
	for(size_t dstIndex = 0; dstIndex < newCapacity; dstIndex++)
	{
		//if we have copied every child for a given depth backtrack up the stack
		//the stackIndex > 0 guard should never trigger for well formed trees as it is safe by invariant
		while((stackIndex > 0) && (stack[stackIndex] == header->k))
		{
			stack[stackIndex] = 0;
			stackIndex--;
		}
		//move the element
		//src == dst prevents copying an element into the same location it currently is. This occurs along the leftmost spine
		if(dstIndex != srcIndex)
		{
			FSDS_genericKTree_copyElement(memBlock, memBlock, dstIndex, srcIndex);
		}
		//move the active bit
		const uint64_t srcBit = (FSDS_genericKTree_getActiveList(memBlock)[srcIndex / 64] >> (srcIndex % 64)) & 1ULL;
		const size_t dstWord = dstIndex / 64;
		const size_t dstBit = dstIndex % 64;
		FSDS_genericKTree_getActiveList(memBlock)[dstWord] = (FSDS_genericKTree_getActiveList(memBlock)[dstWord] & ~(1ULL << dstBit)) | (srcBit << dstBit);
		newSize += srcBit;

		srcIndex++;
		//increment the child counter on the stack
		stack[stackIndex]++;
		//if we are at a leaf increment srcIndex by srcJump as this is where we are removing a subtree
		if(stackIndex == newDepth - 1)
		{
			srcIndex += srcJump;
		}
		//if we are not at a leaf node move down the stack
		else
		{
			stackIndex++;
		}
	}

	//zero any remaining bits as increaseDepth requires it
	for(size_t i = newCapacity; i < tree->memBlock->header.capacity; i++)
	{
		const size_t j = i / 64;
		const size_t k = i % 64;
		const uint64_t mask = ~(1ULL << k);
		FSDS_genericKTree_getActiveList(tree->memBlock)[j] &= mask;
	}

	header->capacity = newCapacity;
	header->size = newSize;
	header->depth = newDepth;

	return FSDS_K_TREE_SUCCESS;
}
int FSDS_DFSKTree_clear(FSDS_DFSKTree* const tree)
{
	if(tree->memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}

	const size_t k = tree->memBlock->header.k;
	const size_t elementSize = tree->memBlock->header.elementSize;
	FSDS_RETERR(FSDS_DFSKTree_destroy(tree));
	FSDS_RETERR(FSDS_DFSKTree_constructDefault(tree, k, elementSize));

	return FSDS_K_TREE_SUCCESS;
}

int FSDS_DFSKTree_calculateIndex(const FSDS_DFSKTree tree, const uint32_t depth, const uint64_t width, size_t* outIndex)
{
	if(tree.memBlock == nullptr)
	{
		return FSDS_K_TREE_ERROR_TREE_NULL_PTR;
	}
	if(outIndex == nullptr)
	{
		return FSDS_K_TREE_ERROR_OUT_NULL_PTR;
	}
	if(depth >= tree.memBlock->header.depth)
	{
		return FSDS_K_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
	}

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
		case FSDS_K_TREE_ERROR_DEPTH_TOO_SMALL:
			return "requested depth is smaller than the current depth";
		case FSDS_K_TREE_ERROR_DEPTH_TOO_BIG:
			return "requested depth is bigger than the current depth";
		case FSDS_K_TREE_ERROR_DEPTH_ZERO:
			return "requested operation would create a tree of depth zero";
		case FSDS_K_TREE_ERROR_TREE_NULL_PTR:
			return "tree is null";
		case FSDS_K_TREE_ERROR_OUT_NULL_PTR:
			return "output parameter is null";
		case FSDS_K_TREE_ERROR_K_TOO_SMALL:
			return "k must be greater than or equal to 2";
		case FSDS_K_TREE_ERROR_ELEM_SIZE_TOO_SMALL:
			return "elementSize must be greater than 0";
		case FSDS_K_TREE_ERROR_PARAMS_INT_OVERFLOW:
			return "Requested parameters create a tree so large it cannot be represented within int64";
		default:
			return "unknown FSDS_KTree error code";
	}
}