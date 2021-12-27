#pragma once

#include <iterator>
#include <algorithm>
#include <limits>

namespace fsds
{
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject = int>
	class basic_list
	{
		public:
			void foo();

			
			using value_type		= T;
			using allocator_type	= Allocator;
			using size_type			= size_t;
			using difference_type	= std::ptrdiff_t;
			using reference			= value_type&;
			using const_reference	= const value_type& ;
			using pointer			= std::allocator_traits<Allocator>::pointer;
			using const_pointer		= std::allocator_traits<Allocator>::const_pointer;
			/*
			//itterator and const itterator
			typedef std::reverse_iterator<iterator> reverse_iterator
			typedef std::reverse_iterator<const_iterator> const_reverse_iterator
			*/

			//constuctors

			

			constexpr basic_list() noexcept(noexcept(Allocator()));
			constexpr explicit basic_list(const Allocator& alloc) noexcept;
			constexpr basic_list(size_type count, const T& value, const Allocator& alloc = Allocator());
			constexpr explicit basic_list(size_type count, const Allocator& alloc = Allocator());
			template<typename InputIt>
			constexpr basic_list(InputIt first, InputIt last, const Allocator& alloc = Allocator());
			constexpr basic_list(const basic_list& other);
			constexpr basic_list(const basic_list& other, const Allocator& alloc);
			constexpr basic_list(basic_list&& other) noexcept;
			constexpr basic_list(basic_list&& other, const Allocator& alloc);
			constexpr basic_list(std::initializer_list<T> init, const Allocator& alloc = Allocator());

			constexpr ~basic_list();

			constexpr basic_list& operator=(const basic_list& other);
			constexpr basic_list& operator=(basic_list&& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<Allocator>::is_always_equal::value);
			constexpr basic_list& operator=(std::initializer_list<T> ilist);

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

			/*
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
			*/

			//capacity

			[[nodiscard]] constexpr bool empty() const noexcept;
			
			constexpr size_type size() const noexcept;
			
			constexpr size_type max_size() const noexcept;
			
			constexpr void reserve(size_type new_cap);
			
			constexpr size_type capacity() const noexcept;
			
			constexpr void shrink_to_fit();

			//modifiers

			constexpr void clear() noexcept;
			
			/*
			constexpr iterator insert(const_iterator pos, const T& value);
			constexpr iterator insert(const_iterator pos, T&& value);
			constexpr iterator insert(const_iterator pos, size_type count, const T& value);
			template<typename InputIt>
			constexpr iterator insert(const_iterator pos, InputIt first, InputIt last);
			constexpr iterator insert(const_iterator pos, std::initializer_list<T> ilist);

			iterator emplace(const_iterator pos, Args&&... args);
			constexpr iterator emplace(const_iterator pos, Args&&... args);

			constexpr iterator erase(const_iterator pos);
			constexpr iterator erase(const_iterator first, const_iterator last);
			*/

			
			constexpr void push_back(const T& value);
			constexpr void push_back(T&& value);

			template<typename... Args>
			constexpr void emplace_back(Args&&... args);

			constexpr void pop_back();

			constexpr void resize(size_type count);
			constexpr void resize(size_type count, const value_type& value);

			constexpr void swap(basic_list& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value || std::allocator_traits<Allocator>::is_always_equal::value);
			
		
		private:
			T* m_data;
			size_type m_size;
			size_type m_capacity;
	};

	/*
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	constexpr bool operator==(const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& lhs, const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	bool operator!=(const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& lhs, const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	bool operator<(const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& lhs, const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	bool operator<=(const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& lhs, const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	bool operator>(const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& lhs, const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& rhs);
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	bool operator>=(const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& lhs, const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& rhs);
	//template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	//constexpr operator<=>(const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& lhs, const fsds::basic_list<T, Allocator, ThreadSycronisationObject>& rhs);
	*/
}

#include "list.inl"