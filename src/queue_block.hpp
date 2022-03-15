#pragma once

template<typename T, size_t maxSize> 
struct QueueBlock
{
	public:
		T data[maxSize];
		constexpr size_t size() const { return maxSize; }
};