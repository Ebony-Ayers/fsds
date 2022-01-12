#pragma once

#include <iterator>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace fsds
{
	template<typename T>
	struct ListIterator
	{
		public:
			using iterator_catagory			= std::contiguous_iterator_tag;
			using size_type					= size_t;
			using difference_type			= std::ptrdiff_t;
			using value_type				= T;
			using pointer					= value_type*;
			using reference					= value_type&;

			ListIterator(pointer ptr);

			reference operator*() const;
			pointer operator->();
			reference operator[](size_type pos) const;

			ListIterator& operator++();
			ListIterator& operator++(int);
			ListIterator& operator--();
			ListIterator& operator--(int);

			ListIterator& operator+=(size_type value);
			ListIterator& operator-=(size_type value);

			pointer getPtr() const;

		private:
			pointer m_ptr;
	};
	template<typename T>
	bool operator==(const ListIterator<T>& lhs, const ListIterator<T>& rhs);
	template<typename T>
	bool operator!=(const ListIterator<T>& lhs, const ListIterator<T>& rhs);
	template<typename T>
	bool operator<(const ListIterator<T>& lhs, const ListIterator<T>& rhs);
	template<typename T>
	bool operator<=(const ListIterator<T>& lhs, const ListIterator<T>& rhs);
	template<typename T>
	bool operator>(const ListIterator<T>& lhs, const ListIterator<T>& rhs);
	template<typename T>
	bool operator>=(const ListIterator<T>& lhs, const ListIterator<T>& rhs);

	template<typename T>
	ListIterator<T> operator+(const ListIterator<T>& lhs, const typename ListIterator<T>::size_type& rhs);
	template<typename T>
	ListIterator<T> operator+(const typename ListIterator<T>::size_type& lhs, const ListIterator<T>& rhs);
	template<typename T>
	ListIterator<T> operator-(const ListIterator<T>& lhs, const typename ListIterator<T>::size_type& rhs);
	template<typename T>
	ListIterator<T> operator-(const typename ListIterator<T>::size_type& lhs, const ListIterator<T>& rhs);
	
	template<typename T, typename Allocator = std::allocator<T>>
	class List
	{
		public:
			using value_type				= T;
			using allocator_type			= Allocator;
			using size_type					= size_t;
			using difference_type			= std::ptrdiff_t;
			using reference					= value_type&;
			using const_reference			= const value_type& ;
			using pointer					= std::allocator_traits<Allocator>::pointer;
			using const_pointer				= std::allocator_traits<Allocator>::const_pointer;
			
			using iterator					= ListIterator<T>;
			using reverse_iterator			= std::reverse_iterator<iterator>;
			using const_iterator			= ListIterator<const T>;
			using const_reverse_iterator	= std::reverse_iterator<const_iterator>;
			

			//constuctors

			constexpr List() noexcept(noexcept(Allocator()));
			constexpr explicit List(const Allocator& alloc) noexcept;
			constexpr List(size_type count, const T& value, const Allocator& alloc = Allocator());
			constexpr explicit List(size_type count, const Allocator& alloc = Allocator());
			template<typename InputIt>
			constexpr List(InputIt first, InputIt last, const Allocator& alloc = Allocator());
			constexpr List(const List& other);
			constexpr List(const List& other, const Allocator& alloc);
			constexpr List(List&& other) noexcept;
			constexpr List(List&& other, const Allocator& alloc);
			constexpr List(std::initializer_list<T> init, const Allocator& alloc = Allocator());

			constexpr ~List();

			constexpr List& operator=(const List& other);
			constexpr List& operator=(List&& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<Allocator>::is_always_equal::value);
			constexpr List& operator=(std::initializer_list<T> ilist);

			constexpr void assign(size_type count, const T& value);
			template<typename InputIt>
			constexpr void assign(InputIt first, InputIt last);
			constexpr void assign(std::initializer_list<T> ilist);

			constexpr allocator_type get_allocator() const noexcept;
			
			//element access

			constexpr reference at(size_type pos);
			constexpr const_reference at(size_type pos) const;

			constexpr reference operator[](size_type pos);
			constexpr const_reference operator[](size_type pos) const;

			constexpr reference front();
			constexpr const_reference front() const;

			constexpr reference back();
			constexpr const_reference back() const;

			constexpr T* data() noexcept;
			constexpr const T* data() const noexcept;

			//iterators

			constexpr iterator begin() noexcept;
			constexpr const_iterator begin() const noexcept;
			constexpr const_iterator cbegin() const noexcept;

			constexpr iterator end() noexcept;
			constexpr const_iterator end() const noexcept;
			constexpr const_iterator cend() const noexcept;

			constexpr reverse_iterator rbegin() noexcept;
			constexpr const_reverse_iterator rbegin() const noexcept;
			constexpr const_reverse_iterator crbegin() const noexcept;

			constexpr reverse_iterator rend() noexcept;
			constexpr const_reverse_iterator rend() const noexcept;
			constexpr const_reverse_iterator crend() const noexcept;

			//capacity

			[[nodiscard]] constexpr bool empty() const noexcept;
			
			constexpr size_type size() const noexcept;
			
			constexpr size_type max_size() const noexcept;
			
			constexpr void reserve(size_type new_cap);
			
			constexpr size_type capacity() const noexcept;
			
			constexpr void shrink_to_fit();

			//modifiers

			constexpr void clear() noexcept;
			
			constexpr iterator insert(const_iterator pos, const T& value);
			constexpr iterator insert(const_iterator pos, T&& value);
			constexpr iterator insert(const_iterator pos, size_type count, const T& value);
			template<typename InputIt>
			constexpr iterator insert(const_iterator pos, InputIt first, InputIt last);
			constexpr iterator insert(const_iterator pos, std::initializer_list<T> ilist);

			template<typename... Args>
			constexpr iterator emplace(const_iterator pos, Args&&... args);

			constexpr iterator erase(const_iterator pos);
			constexpr iterator erase(const_iterator first, const_iterator last);
			
			constexpr void push_back(const T& value);
			constexpr void push_back(T&& value);

			template<typename... Args>
			constexpr void emplace_back(Args&&... args);

			constexpr void pop_back();

			constexpr void resize(size_type count);
			constexpr void resize(size_type count, const value_type& value);

			constexpr void swap(List& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value || std::allocator_traits<Allocator>::is_always_equal::value);
			
		
		private:
			template<bool shouldShrink = false>
			constexpr void reallocate(const size_type& newCapacity);
			constexpr void allocate(const size_type& newCapacity);

			constexpr fts::ReadWriteLock* getLockPointer();

			T* m_data;
			size_type m_size;
			size_type m_capacity;
	};
	
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator!=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator<(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator<=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator>(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator>=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr auto operator<=>(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs);






	template<typename T>
	struct TS_ListIterator
	{
		public:
			using iterator_catagory			= std::contiguous_iterator_tag;
			using size_type					= size_t;
			using difference_type			= std::ptrdiff_t;
			using value_type				= T;
			using pointer					= value_type*;
			using reference					= value_type&;

			TS_ListIterator(pointer ptr, fts::ReadWriteLock* lockPtr);

			reference operator*();
			pointer operator->();
			reference operator[](size_type pos);

			TS_ListIterator& operator++();
			TS_ListIterator& operator++(int);
			TS_ListIterator& operator--();
			TS_ListIterator& operator--(int);

			TS_ListIterator& operator+=(size_type value);
			TS_ListIterator& operator-=(size_type value);

			pointer getPtr();

		private:
			pointer m_ptr;
			fts::ReadWriteLock* m_lockPtr;
	};
	template<typename T>
	bool operator==(const TS_ListIterator<T>& lhs, const TS_ListIterator<T>& rhs);
	template<typename T>
	bool operator!=(const TS_ListIterator<T>& lhs, const TS_ListIterator<T>& rhs);
	template<typename T>
	bool operator<(const TS_ListIterator<T>& lhs, const TS_ListIterator<T>& rhs);
	template<typename T>
	bool operator<=(const TS_ListIterator<T>& lhs, const TS_ListIterator<T>& rhs);
	template<typename T>
	bool operator>(const TS_ListIterator<T>& lhs, const TS_ListIterator<T>& rhs);
	template<typename T>
	bool operator>=(const TS_ListIterator<T>& lhs, const TS_ListIterator<T>& rhs);

	template<typename T>
	TS_ListIterator<T> operator+(const TS_ListIterator<T>& lhs, const typename TS_ListIterator<T>::size_type& rhs);
	template<typename T>
	TS_ListIterator<T> operator+(const typename TS_ListIterator<T>::size_type& lhs, const TS_ListIterator<T>& rhs);
	template<typename T>
	TS_ListIterator<T> operator-(const TS_ListIterator<T>& lhs, const typename TS_ListIterator<T>::size_type& rhs);
	template<typename T>
	TS_ListIterator<T> operator-(const typename TS_ListIterator<T>::size_type& lhs, const TS_ListIterator<T>& rhs);

	
	struct ReadLockGuard
	{
		fts::ReadWriteLock* pLock;
		ReadLockGuard(fts::ReadWriteLock* lock) : pLock(lock) { this->pLock->readLock(); }
		~ReadLockGuard() { this->pLock->readUnlock(); }
	};
	struct WriteLockGuard
	{
		fts::ReadWriteLock* pLock;
		WriteLockGuard(fts::ReadWriteLock* lock) : pLock(lock) { this->pLock->writeLock(); }
		~WriteLockGuard() { this->pLock->writeUnlock(); }
	};

	template<typename T, typename Allocator = std::allocator<T>>
	class TS_List
	{
		public:
			using value_type				= T;
			using allocator_type			= Allocator;
			using size_type					= size_t;
			using difference_type			= std::ptrdiff_t;
			using reference					= value_type&;
			using const_reference			= const value_type& ;
			using pointer					= std::allocator_traits<Allocator>::pointer;
			using const_pointer				= std::allocator_traits<Allocator>::const_pointer;
			
			using iterator					= TS_ListIterator<T>;
			using reverse_iterator			= std::reverse_iterator<iterator>;
			using const_iterator			= TS_ListIterator<const T>;
			using const_reverse_iterator	= std::reverse_iterator<const_iterator>;
			

			//constuctors

			constexpr TS_List() noexcept(noexcept(Allocator()));
			constexpr explicit TS_List(const Allocator& alloc) noexcept;
			constexpr TS_List(size_type count, const T& value, const Allocator& alloc = Allocator());
			constexpr explicit TS_List(size_type count, const Allocator& alloc = Allocator());
			template<typename InputIt>
			constexpr TS_List(InputIt first, InputIt last, const Allocator& alloc = Allocator());
			constexpr TS_List(const TS_List& other);
			constexpr TS_List(const TS_List& other, const Allocator& alloc);
			constexpr TS_List(TS_List&& other) noexcept;
			constexpr TS_List(TS_List&& other, const Allocator& alloc);
			constexpr TS_List(std::initializer_list<T> init, const Allocator& alloc = Allocator());

			constexpr ~TS_List();

			constexpr TS_List& operator=(const TS_List& other);
			constexpr TS_List& operator=(TS_List&& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<Allocator>::is_always_equal::value);
			constexpr TS_List& operator=(std::initializer_list<T> iTS_List);

			constexpr void assign(size_type count, const T& value);
			template<typename InputIt>
			constexpr void assign(InputIt first, InputIt last);
			constexpr void assign(std::initializer_list<T> iTS_List);

			constexpr allocator_type get_allocator() noexcept;
			
			//element access

			constexpr reference at(size_type pos);

			constexpr reference operator[](size_type pos);

			constexpr reference front();

			constexpr reference back();

			constexpr T* data() noexcept;

			//iterators

			constexpr iterator begin() noexcept;
			constexpr const_iterator cbegin() noexcept;

			constexpr iterator end() noexcept;
			constexpr const_iterator cend() noexcept;

			constexpr reverse_iterator rbegin() noexcept;
			constexpr const_reverse_iterator crbegin() noexcept;

			constexpr reverse_iterator rend() noexcept;
			constexpr const_reverse_iterator crend() noexcept;

			//capacity

			[[nodiscard]] constexpr bool empty() noexcept;
			
			constexpr size_type size() noexcept;
			
			constexpr size_type max_size() noexcept;
			
			constexpr void reserve(size_type new_cap);
			
			constexpr size_type capacity() noexcept;
			
			constexpr void shrink_to_fit();

			//modifiers

			constexpr void clear() noexcept;
			
			constexpr iterator insert(const_iterator pos, const T& value);
			constexpr iterator insert(const_iterator pos, T&& value);
			constexpr iterator insert(const_iterator pos, size_type count, const T& value);
			template<typename InputIt>
			constexpr iterator insert(const_iterator pos, InputIt first, InputIt last);
			constexpr iterator insert(const_iterator pos, std::initializer_list<T> iTS_List);

			template<typename... Args>
			constexpr iterator emplace(const_iterator pos, Args&&... args);

			constexpr iterator erase(const_iterator pos);
			constexpr iterator erase(const_iterator first, const_iterator last);
			
			constexpr void push_back(const T& value);
			constexpr void push_back(T&& value);

			template<typename... Args>
			constexpr void emplace_back(Args&&... args);

			constexpr void pop_back();

			constexpr void resize(size_type count);
			constexpr void resize(size_type count, const value_type& value);

			constexpr void swap(TS_List& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value || std::allocator_traits<Allocator>::is_always_equal::value);
			
			constexpr void flush();
		
		private:
			template<bool shouldShrink = false>
			constexpr void reallocate(const size_type& newCapacity);
			constexpr void allocate(const size_type& newCapacity);

			constexpr fts::ReadWriteLock* getLockPointer();

			T* m_data;
			size_type m_size;
			size_type m_capacity;

			fts::ReadWriteLock m_lock;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator!=(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator<(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator<=(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator>(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator>=(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr auto operator<=>(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs);
}

#include "list.inl"