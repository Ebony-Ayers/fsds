#pragma once

#include "queue_block.hpp"

namespace fsds
{
	template<typename T, size_t blockSize = 16, typename Allocator = std::allocator<QueueBlock<T, blockSize>*>>
	class SPMCQueue
	{
		public:
			//constexpr static size_t blockSize = 16;
			using Block = QueueBlock<T, blockSize>; 

			constexpr SPMCQueue() noexcept(noexcept(Allocator()));
			constexpr explicit SPMCQueue(size_t defaultSize, const Allocator& alloc = Allocator());
			constexpr explicit SPMCQueue(const Allocator& alloc) noexcept = delete;
			constexpr SPMCQueue(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr SPMCQueue(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr SPMCQueue(const SPMCQueue& other) = delete;
			constexpr SPMCQueue(const SPMCQueue& other, const Allocator& alloc) = delete;
			constexpr SPMCQueue(SPMCQueue&& other) noexcept = delete;
			constexpr SPMCQueue(SPMCQueue&& other, const Allocator& alloc) = delete;
			constexpr SPMCQueue(std::initializer_list<T> init, const Allocator& alloc = Allocator()) = delete;
			constexpr ~SPMCQueue();

			constexpr void enqueue(const T& value);
			constexpr void enqueueBlock(Block* block);
			constexpr T dequeue();
			constexpr bool tryDequeue(T* dest);
			[[nodiscard]] constexpr T& front();

			constexpr T& operator[](size_t pos);

			[[nodiscard]] constexpr bool isEmpty() noexcept;
			[[nodiscard]] constexpr size_t size() noexcept;
			[[nodiscard]] constexpr size_t capacity() noexcept;

			constexpr void reserve(size_t newCap);
			constexpr void clear();

			[[nodiscard]] constexpr bool dataReferenceEquality(SPMCQueue<T, blockSize, Allocator>& other);
			[[nodiscard]] constexpr bool valueEquality(SPMCQueue<T, blockSize, Allocator>& other);

		private:
			constexpr static size_t sm_baseAllocation = 16;
			constexpr void reallocate(size_t newSize);
			constexpr void allocateNewBlock();
			constexpr T& atNoLock(const size_t& pos) const;
			constexpr bool dataReferenceEqualityNoLock(SPMCQueue<T, blockSize, Allocator>& other);
			fts::SpinLock* getLock();

			Block** m_data;
			size_t m_frontBlockOffset;
			size_t m_firstBlock;
			size_t m_lastBlock;

			size_t m_size;
			//the numebr of active block
			size_t m_numBlocks;
			//capacity refers the the number of blocks allocated not the number of elements allocated
			size_t m_capacity;

			fts::SpinSemaphore m_dataAccessLock;
			fts::SpinLock m_controlBlockLock;
	};

	template<typename T, size_t blockSize = 16, typename Allocator = std::allocator<QueueBlock<T, blockSize>>>
	constexpr bool operator==(SPMCQueue<T, blockSize, Allocator>& lhs, SPMCQueue<T, blockSize, Allocator>& rhs);
	template<typename T, size_t blockSize = 16, typename Allocator = std::allocator<QueueBlock<T, blockSize>>>
	constexpr bool operator!=(SPMCQueue<T, blockSize, Allocator>& lhs, SPMCQueue<T, blockSize, Allocator>& rhs);
}

#include "spmc_queue.inl"