#pragma once

#include <iterator>
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace fsds
{
	template<typename T>
	struct BasicListIterator
	{
		public:
			using iterator_catagory			= std::contiguous_iterator_tag;
			using size_type					= size_t;
			using difference_type			= std::ptrdiff_t;
			using value_type				= T;
			using pointer					= value_type*;
			using reference					= value_type&;

			BasicListIterator(pointer ptr);

			reference operator*() const;
			pointer operator->();
			reference operator[](size_type pos) const;

			BasicListIterator& operator++();
			BasicListIterator& operator++(int);
			BasicListIterator& operator--();
			BasicListIterator& operator--(int);

			BasicListIterator& operator+=(size_type value);
			BasicListIterator& operator-=(size_type value);

		private:
			pointer m_ptr;
	};
	template<typename T>
	bool operator==(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs);
	template<typename T>
	bool operator!=(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs);
	template<typename T>
	bool operator<(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs);
	template<typename T>
	bool operator<=(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs);
	template<typename T>
	bool operator>(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs);
	template<typename T>
	bool operator>=(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs);

	template<typename T>
	BasicListIterator<T> operator+(const BasicListIterator<T>& lhs, const typename BasicListIterator<T>::size_type& rhs);
	template<typename T>
	BasicListIterator<T> operator+(const typename BasicListIterator<T>::size_type& lhs, const BasicListIterator<T>& rhs);
	template<typename T>
	BasicListIterator<T> operator-(const BasicListIterator<T>& lhs, const typename BasicListIterator<T>::size_type& rhs);
	template<typename T>
	BasicListIterator<T> operator-(const typename BasicListIterator<T>::size_type& lhs, const BasicListIterator<T>& rhs);
	
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject = int>
	class BasicList
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
			
			using iterator					= BasicListIterator<T>;
			using reverse_iterator			= std::reverse_iterator<iterator>;
			using const_iterator			= BasicListIterator<const T>;
			using const_reverse_iterator	= std::reverse_iterator<const_iterator>;
			

			//constuctors

			constexpr BasicList() noexcept(noexcept(Allocator()));
			constexpr explicit BasicList(const Allocator& alloc) noexcept;
			constexpr BasicList(size_type count, const T& value, const Allocator& alloc = Allocator());
			constexpr explicit BasicList(size_type count, const Allocator& alloc = Allocator());
			template<typename InputIt>
			constexpr BasicList(InputIt first, InputIt last, const Allocator& alloc = Allocator());
			constexpr BasicList(const BasicList& other);
			constexpr BasicList(const BasicList& other, const Allocator& alloc);
			constexpr BasicList(BasicList&& other) noexcept;
			constexpr BasicList(BasicList&& other, const Allocator& alloc);
			constexpr BasicList(std::initializer_list<T> init, const Allocator& alloc = Allocator());

			constexpr ~BasicList();

			constexpr BasicList& operator=(const BasicList& other);
			constexpr BasicList& operator=(BasicList&& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<Allocator>::is_always_equal::value);
			constexpr BasicList& operator=(std::initializer_list<T> ilist);

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

			constexpr void swap(BasicList& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value || std::allocator_traits<Allocator>::is_always_equal::value);
			
		
		//private:
			template<bool shouldShrink>
			constexpr void reallocate(const size_type& newCapacity);
			constexpr void allocate(const size_type& newCapacity);

			T* m_data;
			size_type m_size;
			size_type m_capacity;
	};

	
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject = int>
	constexpr bool operator==(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject = int>
	bool operator!=(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject = int>
	bool operator<(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject = int>
	bool operator<=(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject = int>
	bool operator>(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject = int>
	bool operator>=(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject = int>
	constexpr auto operator<=>(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs);
	
}

#include "list.inl"