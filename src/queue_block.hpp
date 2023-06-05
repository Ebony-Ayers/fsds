#ifndef QUEUE_BLOCK_HPP_HEADER_GUARD
#define QUEUE_BLOCK_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

template<typename T, size_t maxSize> 
struct QueueBlock
{
	public:
		T data[maxSize];
		constexpr size_t size() const { return maxSize; }
};

#endif //#ifndef QUEUE_BLOCK_HPP_HEADER_GUARD