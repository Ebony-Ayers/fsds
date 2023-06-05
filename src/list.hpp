#ifndef LIST_HPP_HEADER_GUARD
#define LIST_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

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
			constexpr List() noexcept(noexcept(Allocator()));	//default constructor
			constexpr explicit List(const size_t& count);		//constructor specifying initial capacity
			constexpr List(const List& other);					//copy constructor
			constexpr List(List&& other) noexcept;				//move constructor
			constexpr List(std::initializer_list<T> init, const Allocator& alloc = Allocator());	//initializer list
			constexpr explicit List(const Allocator& alloc) noexcept = delete;
			constexpr List(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr List(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr List(const List& other, const Allocator& alloc) = delete;
			constexpr List(List&& other, const Allocator& alloc) = delete;
			constexpr ~List();

			constexpr List& operator=(const List& other);
			constexpr List& operator=(List&& other) noexcept;

			constexpr Allocator getAllocator() const noexcept;

			constexpr T& operator[](size_t pos);
			constexpr const T& operator[](size_t pos) const;

			constexpr T& front();
			constexpr const T& front() const;
			constexpr T& back();
			constexpr const T& back() const;

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

			constexpr bool valueEquality(const List<T, Allocator>& other) const;

			constexpr void deepCopy(List<T, Allocator>& dest) const;
		
		private:
			constexpr static size_t sm_baseAllocation = 16;
			
			void reallocate(size_t newSize);

			T* m_data;
			size_t m_size;
			size_t m_capacity;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator!=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);

	template<typename T, typename Allocator = std::allocator<T>>
	class ts_List
	{
		public:
			constexpr ts_List() noexcept(noexcept(Allocator()));	//default constructor
			constexpr explicit ts_List(const size_t& count);		//constructor specifying initial capacity
			constexpr ts_List(const ts_List& other);					//copy constructor
			constexpr ts_List(ts_List&& other) noexcept;				//move constructor
			constexpr ts_List(std::initializer_list<T> init, const Allocator& alloc = Allocator());	//initializer list
			constexpr explicit ts_List(const Allocator& alloc) noexcept = delete;
			constexpr ts_List(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr ts_List(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr ts_List(const ts_List& other, const Allocator& alloc) = delete;
			constexpr ts_List(ts_List&& other, const Allocator& alloc) = delete;
			constexpr ~ts_List();

			constexpr Allocator getAllocator() const noexcept;

			constexpr T* operator[](size_t pos);

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

			constexpr bool valueEquality(const ts_List<T, Allocator>& other);
		
		private:
			fsds::List<T,Allocator> m_list;
			fts::ReadWriteLock m_lock;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(const ts_List<T, Allocator>& lhs, const ts_List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator!=(const ts_List<T, Allocator>& lhs, const ts_List<T, Allocator>& rhs);
}

#include "list.inl"
#include "ts_list.inl"

#endif //#ifndef LIST_HPP_HEADER_GUARD