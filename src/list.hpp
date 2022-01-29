#pragma once

#include <iterator>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace fsds
{
	template<typename T, typename Allocator = std::allocator<T>>
	class List
	{
		public:
			constexpr List() noexcept(noexcept(Allocator()));
			constexpr explicit List(const Allocator& alloc) noexcept = delete;
			constexpr List(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			constexpr explicit List(size_t count, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr List(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr List(const List& other) = delete;
			constexpr List(const List& other, const Allocator& alloc) = delete;
			constexpr List(List&& other) noexcept = delete;
			constexpr List(List&& other, const Allocator& alloc) = delete;
			constexpr List(std::initializer_list<T> init, const Allocator& alloc = Allocator());
			constexpr ~List();

			constexpr Allocator getAllocator() const noexcept;

			constexpr T* operator[](size_t pos);
			constexpr const T* operator[](size_t pos) const;

			constexpr T* front();
			constexpr const T* front() const;
			constexpr T* back();
			constexpr const T* back() const;

			constexpr T* data();
			constexpr const T* data() const;

			[[nodiscard]] constexpr bool isEmpty() const noexcept;
			constexpr size_t size() const noexcept;
			constexpr size_t maxSize() const noexcept;
			constexpr size_t capacity() const noexcept;

			constexpr void reserve(size_t newCap);
			constexpr void clear();
			
			constexpr void append(const T& value);
			template<typename... Args>
			constexpr void appendConstruct(Args&&... args);
			constexpr void prepend(const T& value);
			template<typename... Args>
			constexpr void prependConstruct(Args&&... args);
			constexpr void insert(size_t pos, const T& value);
			template<typename... Args>
			constexpr void insertConstruct(size_t pos, Args&&... args);

			constexpr void remove(size_t pos);
			constexpr void removeBack();
			constexpr void removeFront();

			constexpr bool dataReferenceEquality(const List<T, Allocator>& other);
			constexpr bool valueEquality(const List<T, Allocator>& other);
		
		private:
			void reallocate(size_t newSize);

			T* m_data;
			size_t m_size;
			size_t m_capacity;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator!=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);

	class ts_ListReadLockGuard
	{
		public:
			ts_ListReadLockGuard(fts::ReadWriteLock* lock) : m_lock(lock) { m_lock->readLock(); }
			~ts_ListReadLockGuard() { m_lock->readUnlock(); }
		
		private:
			fts::ReadWriteLock* m_lock;
	};
	class ts_ListWriteLockGuard
	{
		public:
			ts_ListWriteLockGuard(fts::ReadWriteLock* lock) : m_lock(lock) { m_lock->writeLock(); }
			~ts_ListWriteLockGuard() { m_lock->writeUnlock(); }
		
		private:
			fts::ReadWriteLock* m_lock;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	class ts_List
	{
		public:
			constexpr ts_List() noexcept(noexcept(Allocator()));
			constexpr explicit ts_List(const Allocator& alloc) noexcept = delete;
			constexpr ts_List(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			constexpr explicit ts_List(size_t count, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr ts_List(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr ts_List(const ts_List& other) = delete;
			constexpr ts_List(const ts_List& other, const Allocator& alloc) = delete;
			constexpr ts_List(ts_List&& other) noexcept = delete;
			constexpr ts_List(ts_List&& other, const Allocator& alloc) = delete;
			constexpr ts_List(std::initializer_list<T> init, const Allocator& alloc = Allocator());
			constexpr ~ts_List();

			constexpr Allocator getAllocator() const noexcept;

			constexpr T* operator[](size_t pos);
			constexpr const T* operator[](size_t pos) const;

			constexpr T* front();
			constexpr T* back();

			constexpr T* data();

			[[nodiscard]] constexpr bool isEmpty() noexcept;
			constexpr size_t size() noexcept;
			constexpr size_t maxSize() noexcept;
			constexpr size_t capacity() noexcept;

			constexpr void reserve(size_t newCap);
			constexpr void clear();
			
			constexpr void append(const T& value);
			template<typename... Args>
			constexpr void appendConstruct(Args&&... args);
			constexpr void prepend(const T& value);
			template<typename... Args>
			constexpr void prependConstruct(Args&&... args);
			constexpr void insert(size_t pos, const T& value);
			template<typename... Args>
			constexpr void insertConstruct(size_t pos, Args&&... args);

			constexpr void remove(size_t pos);
			constexpr void removeBack();
			constexpr void removeFront();

			constexpr bool dataReferenceEquality(const ts_List<T, Allocator>& other);
			constexpr bool valueEquality(const ts_List<T, Allocator>& other);
		
		private:
			void reallocate(size_t newSize);

			T* m_data;
			size_t m_size;
			size_t m_capacity;
			fts::ReadWriteLock m_lock;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(const ts_List<T, Allocator>& lhs, const ts_List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator!=(const ts_List<T, Allocator>& lhs, const ts_List<T, Allocator>& rhs);
}

#include "list.inl"
#include "ts_list.inl"