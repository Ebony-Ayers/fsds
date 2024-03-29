#ifndef FINITE_PQUEUE_HPP_HEADER_GUARD
#define FINITE_PQUEUE_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include <array>
#include "seperate_data_list.hpp"
#include <iostream>
#include <algorithm>

namespace fsds
{
	template<typename T, size_t numPriorities, size_t blockSize = 8 * numPriorities>
	class FinitePQueue
	{
		public:
			FinitePQueue();
			constexpr FinitePQueue(size_t count, const T& value) = delete;
			template<typename InputIt>
			constexpr FinitePQueue(InputIt first, InputIt last) = delete;
			constexpr FinitePQueue(const FinitePQueue& other) = delete;
			constexpr FinitePQueue(FinitePQueue&& other) = delete;
			FinitePQueue(std::initializer_list<T> init) = delete;
			~FinitePQueue();

			void enqueue(const T& value, size_t priority);
			void enqueueFront(const T& value, size_t priority);
			T dequeue();
			T dequeuePriority(size_t priority);
			bool tryDequeue(T* dest, size_t priority);
			[[nodiscard]] T& front() const;
			[[nodiscard]] T& frontPriority(size_t priority) const;
			[[nodiscard]] size_t getCurrentHighestPriority() const;

			[[nodiscard]] constexpr bool isEmpty() const noexcept;
			[[nodiscard]] constexpr size_t size() const noexcept;
			[[nodiscard]] constexpr size_t capacity() const noexcept;

			void clear();

			[[nodiscard]] bool valueEquality(FinitePQueue<T, numPriorities, blockSize>& other);

		//private:
			constexpr static size_t sm_blockSize = 8 * numPriorities;

			void makeNewBlock();

			struct Node
			{
				T data;
				Node* next;
				Node* previous;
			};

			std::array<Node*, numPriorities> m_queueHeads;
			std::array<Node*, numPriorities> m_queueTails;
			std::array<size_t, numPriorities> m_queueSizes;

			fsds::SPSCQueue<Node*> m_availableNodes;

			fsds::SeperateDataList<Node*> m_rawNodeStorage;

			size_t m_totalSize;
	};

	template<typename T, size_t numPriorities, size_t blockSize = 8 * numPriorities>
	bool operator==(FinitePQueue<T, numPriorities, blockSize>& lhs, FinitePQueue<T, numPriorities, blockSize>& rhs);
	template<typename T, size_t numPriorities, size_t blockSize = 8 * numPriorities>
	bool operator!=(FinitePQueue<T, numPriorities, blockSize>& lhs, FinitePQueue<T, numPriorities, blockSize>& rhs);
}

#include "finite_pqueue.inl"

#endif //#ifndef FINITE_PQUEUE_HPP_HEADER_GUARD