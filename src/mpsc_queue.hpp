#pragma once
#include "fsds_options.hpp"

#include "queue_block.hpp"

namespace fsds
{
	template<typename T, size_t blockSize = 16, typename Allocator = std::allocator<QueueBlock<T, blockSize>*>>
	class MPSCQueue
	{
		public:
			using Block = QueueBlock<T, blockSize>; 

			MPSCQueue() noexcept(noexcept(Allocator()));
			MPSCQueue(size_t defaultSize, const Allocator& alloc = Allocator());
			constexpr explicit MPSCQueue(const Allocator& alloc) noexcept = delete;
			constexpr MPSCQueue(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr MPSCQueue(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr MPSCQueue(const MPSCQueue& other) = delete;
			constexpr MPSCQueue(const MPSCQueue& other, const Allocator& alloc) = delete;
			constexpr MPSCQueue(MPSCQueue&& other) noexcept = delete;
			constexpr MPSCQueue(MPSCQueue&& other, const Allocator& alloc) = delete;
			MPSCQueue(std::initializer_list<T> init, const Allocator& alloc = Allocator()) = delete;
			~MPSCQueue();

			void enqueue(const T& value);
			T dequeue();
			bool dequeueBlock(Block* dest);
			bool tryDequeue(T* dest);
			[[nodiscard]] T& front();

			T& operator[](size_t pos);

			[[nodiscard]] constexpr bool isEmpty() noexcept;
			[[nodiscard]] constexpr size_t size() noexcept;
			[[nodiscard]] constexpr size_t capacity() noexcept;

			void reserve(size_t newCap);
			void clear();

			[[nodiscard]] bool dataReferenceEquality(MPSCQueue<T, blockSize, Allocator>& other);
			[[nodiscard]] bool valueEquality(MPSCQueue<T, blockSize, Allocator>& other);

		private:
			constexpr static size_t sm_baseAllocation = 16;
			constexpr void reallocate(size_t newSize);
			constexpr void allocateNewBlock();
			constexpr T& atNoLock(const size_t& pos) const;
			constexpr bool dataReferenceEqualityNoLock(MPSCQueue<T, blockSize, Allocator>& other);
			fts::SpinLock* getLock();

			//array of pointers to blocks of data
			Block** m_data;
			//the offset of the start of the data in the frist block
			//if the first three entries in the block were dequeued then m_frontBlockOffset would be equal to 3 indecating the next valid peice of data is at the 4th index
			size_t m_frontBlockOffset;
			//index of the first block that contains valid data
			size_t m_firstBlock;
			//index of the last block that contains valid data
			size_t m_lastBlock;

			//number of elements in the entire queue
			size_t m_size;
			//the numebr of active block
			size_t m_numBlocks;
			//capacity refers the the number of blocks allocated not the number of elements allocated
			size_t m_capacity;

			fts::SpinSemaphore m_dataAccessLock;
			fts::SpinLock m_controlBlockLock;
	};

	template<typename T, size_t blockSize = 16, typename Allocator = std::allocator<QueueBlock<T, blockSize>>>
	bool operator==(MPSCQueue<T, blockSize, Allocator>& lhs, MPSCQueue<T, blockSize, Allocator>& rhs);
	template<typename T, size_t blockSize = 16, typename Allocator = std::allocator<QueueBlock<T, blockSize>>>
	bool operator!=(MPSCQueue<T, blockSize, Allocator>& lhs, MPSCQueue<T, blockSize, Allocator>& rhs);
}

#include "mpsc_queue.inl"