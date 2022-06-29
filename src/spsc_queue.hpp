#pragma once
#include "fsds_options.hpp"

namespace fsds
{
	template<typename T, typename Allocator = std::allocator<T>>
	class SPSCQueue
	{
		public:
			constexpr SPSCQueue() noexcept(noexcept(Allocator()));
			constexpr explicit SPSCQueue(size_t defaultSize);
			constexpr explicit SPSCQueue(const Allocator& alloc) noexcept = delete;
			constexpr SPSCQueue(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr SPSCQueue(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr SPSCQueue(const SPSCQueue& other) = delete;
			constexpr SPSCQueue(const SPSCQueue& other, const Allocator& alloc) = delete;
			constexpr SPSCQueue(SPSCQueue&& other) noexcept = delete;
			constexpr SPSCQueue(SPSCQueue&& other, const Allocator& alloc) = delete;
			constexpr SPSCQueue(std::initializer_list<T> init, const Allocator& alloc = Allocator()) = delete;
			constexpr ~SPSCQueue();

			constexpr void enqueue(const T& value);
			constexpr T dequeue();
			constexpr T& front();

			constexpr T& operator[](size_t pos);
			constexpr const T& operator[](size_t pos) const;

			[[nodiscard]] constexpr bool isEmpty() const noexcept;
			constexpr size_t size() const noexcept;
			constexpr size_t capacity() const noexcept;

			constexpr void reserve(const size_t& newCap);
			constexpr void clear();

			constexpr bool dataReferenceEquality(const SPSCQueue<T, Allocator>& other) = delete;
			constexpr bool valueEquality(const SPSCQueue<T, Allocator>& other) const;

		private:
			constexpr static size_t sm_baseAllocation = 16;
			constexpr void reallocate(const size_t& newSize);

			T* m_data;
			size_t m_frontOffset;
			size_t m_backOffset;

			size_t m_size;
			size_t m_capacity;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(const SPSCQueue<T, Allocator>& lhs, const SPSCQueue<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator!=(const SPSCQueue<T, Allocator>& lhs, const SPSCQueue<T, Allocator>& rhs);
}

#include "spsc_queue.inl"