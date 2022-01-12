namespace fsds
{
	template<typename T>
	ListIterator<T>::ListIterator(pointer ptr)
	: m_ptr(ptr)
	{}
	
	template<typename T>
	ListIterator<T>::reference ListIterator<T>::operator*() const
	{
		return *(this->m_ptr);
	}
	template<typename T>
	ListIterator<T>::pointer ListIterator<T>::operator->()
	{
		return this->m_ptr;
	}
	template<typename T>
	ListIterator<T>::reference ListIterator<T>::operator[](size_type pos) const
	{
		return this->m_ptr[pos];
	}

	template<typename T>
	ListIterator<T>& ListIterator<T>::operator++()
	{
		this->m_ptr++;
		return this;
	}
	template<typename T>
	ListIterator<T>& ListIterator<T>::operator++(int)
	{
		auto previous = *this;
		this->m_ptr++;
		return previous;
	}
	template<typename T>
	ListIterator<T>& ListIterator<T>::operator--()
	{
		this->m_ptr--;
		return this;
	}
	template<typename T>
	ListIterator<T>& ListIterator<T>::operator--(int)
	{
		auto previous = *this;
		this->m_ptr--;
		return previous;
	}

	template<typename T>
	ListIterator<T>& ListIterator<T>::operator+=(size_type value)
	{
		this->m_ptr += value;
		return *this;
	}
	template<typename T>
	ListIterator<T>& ListIterator<T>::operator-=(size_type value)
	{
		this->m_ptr -= value;
		return *this;
	}

	template<typename T>
	ListIterator<T>::pointer ListIterator<T>::getPtr() const
	{
		return this->m_ptr;
	}

	template<typename T>
	bool operator==(const ListIterator<T>& lhs, const ListIterator<T>& rhs)
	{
		return lhs.getPtr() == rhs.getPtr();
	}
	template<typename T>
	bool operator!=(const ListIterator<T>& lhs, const ListIterator<T>& rhs)
	{
		return lhs.getPtr() != rhs.getPtr();
	}
	template<typename T>
	bool operator<(const ListIterator<T>& lhs, const ListIterator<T>& rhs)
	{
		return lhs.getPtr() < rhs.getPtr();
	}
	template<typename T>
	bool operator<=(const ListIterator<T>& lhs, const ListIterator<T>& rhs)
	{
		return lhs.getPtr() <= rhs.getPtr();
	}
	template<typename T>
	bool operator>(const ListIterator<T>& lhs, const ListIterator<T>& rhs)
	{
		return lhs.getPtr() > rhs.getPtr();
	}
	template<typename T>
	bool operator>=(const ListIterator<T>& lhs, const ListIterator<T>& rhs)
	{
		return lhs.getPtr() >= rhs.getPtr();
	}

	template<typename T>
	ListIterator<T> operator+(const ListIterator<T>& lhs, const typename ListIterator<T>::size_type& rhs)
	{
		return ListIterator<T>(lhs.getPtr() + rhs);
	}
	template<typename T>
	ListIterator<T> operator+(const typename ListIterator<T>::size_type& lhs, const ListIterator<T>& rhs)
	{
		return ListIterator<T>(rhs.getPtr() + lhs);
	}
	template<typename T>
	ListIterator<T> operator-(const ListIterator<T>& lhs, const typename ListIterator<T>::size_type& rhs)
	{
		return ListIterator<T>(lhs.getPtr() - rhs);
	}
	template<typename T>
	ListIterator<T> operator-(const typename ListIterator<T>::size_type& lhs, const ListIterator<T>& rhs)
	{
		return ListIterator<T>(rhs.getPtr() - lhs);
	}


	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List() noexcept(noexcept(Allocator()))
	: m_data(nullptr), m_size(0), m_capacity(0)
	{}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(const Allocator& alloc) noexcept
	: m_data(nullptr), m_size(0), m_capacity(0)
	{}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(size_type count, const T& value, const Allocator& alloc)
	: m_data(nullptr), m_size(count), m_capacity(count)
	{
		this->m_data = alloc.allocate(count);
		std::fill(this->m_data, this->m_data + this->m_size, value);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(size_type count, const Allocator& alloc)
	: m_data(nullptr), m_size(count), m_capacity(count)
	{
		this->m_data = alloc.allocate(count);
		std::fill(this->m_data, this->m_data + this->m_size, value_type());
	}
	template<typename T, typename Allocator>
	template<typename InputIt>
	constexpr List<T, Allocator>::List(InputIt first, InputIt last, const Allocator& alloc)
	{
		auto size = std::distance(first, last);
		this->m_data = alloc.allocate(size);
		std::copy(first, last, this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(const List& other)
	: m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		Allocator alloc;
		this->m_data = alloc.allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(const List& other, const Allocator& alloc)
	: m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		this->m_data = alloc.allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(List&& other) noexcept
	: m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(List&& other, const Allocator& alloc)
	: m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(std::initializer_list<T> init, const Allocator& alloc)
	: m_data(nullptr), m_size(init.size()), m_capacity(init.size())
	{
		this->m_data = alloc.allocate(init.size());
		std::copy(init.begin(), init.end(), this->m_size);
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::~List()
	{
		if(this->m_data != nullptr)
		{
			Allocator alloc;
			alloc.deallocate(this->m_data, this->m_capacity);
		}
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>& List<T, Allocator>::operator=(const List& other)
	{
		this->allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>& List<T, Allocator>::operator=(List&& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<Allocator>::is_always_equal::value)
	{
		this->m_data = other.m_data;
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>& List<T, Allocator>::operator=(std::initializer_list<T> ilist)
	{
		auto size = ilist.size();
		this->allocate(size);
		std::copy(ilist.begin(), ilist.end(), this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::assign(size_type count, const T& value)
	{
		this->allocate(count);
		std::fill(this->m_data, this->m_data + count, value);
		this->m_size = count;
		this->m_capacity = count;
	}
	template<typename T, typename Allocator>
	template<typename InputIt>
	constexpr void List<T, Allocator>::assign(InputIt first, InputIt last)
	{
		auto size = std::distance(first, last);
		this->allocate(size);
		std::copy(first, last, this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::assign(std::initializer_list<T> ilist)
	{
		auto size = ilist.size();
		this->allocate(size);
		std::copy(ilist.begin(), ilist.end(), this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::reference List<T, Allocator>::at(size_type pos)
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::List index out of range");
		}
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_reference List<T, Allocator>::at(size_type pos) const
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::List index out of range");
		}
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::reference List<T, Allocator>::operator[](size_type pos)
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::List index out of range");
		}
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_reference List<T, Allocator>::operator[](size_type pos) const
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::List index out of range");
		}
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::reference List<T, Allocator>::front()
	{
		return this->m_data[0];
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_reference List<T, Allocator>::front() const
	{
		return this->m_data[0];
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::reference List<T, Allocator>::back()
	{
		return this->m_data[this->m_size-1];
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_reference List<T, Allocator>::back() const
	{
		return this->m_data[this->m_size-1];
	}

	template<typename T, typename Allocator>
	constexpr T* List<T, Allocator>::data() noexcept
	{
		return this->m_data;
	}
	template<typename T, typename Allocator>
	constexpr const T* List<T, Allocator>::data() const noexcept
	{
		return this->m_data;
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::iterator List<T, Allocator>::begin() noexcept
	{
		return iterator(this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_iterator List<T, Allocator>::begin() const noexcept
	{
		return const_iterator(this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const noexcept
	{
		return const_iterator(this->m_data);
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::iterator List<T, Allocator>::end() noexcept
	{
		return iterator(this->m_data + this->m_size);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_iterator List<T, Allocator>::end() const noexcept
	{
		return const_iterator(this->m_data + this->m_size);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_iterator List<T, Allocator>::cend() const noexcept
	{
		return const_iterator(this->m_data + this->m_size);
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::reverse_iterator List<T, Allocator>::rbegin() noexcept
	{
		return reverse_iterator(this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rbegin() const noexcept
	{
		return const_reverse_iterator(this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crbegin() const noexcept
	{
		return const_reverse_iterator(this->m_data);
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::reverse_iterator List<T, Allocator>::rend() noexcept
	{
		return reverse_iterator(this->m_data + this->m_size);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rend() const noexcept
	{
		return const_reverse_iterator(this->m_data + this->m_size);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crend() const noexcept
	{
		return const_reverse_iterator(this->m_data + this->m_size);
	}


	template<typename T, typename Allocator>
	[[nodiscard]] constexpr bool List<T, Allocator>::empty() const noexcept
	{
		return (this->m_size == 0);
	}
	
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::size_type List<T, Allocator>::size() const noexcept
	{
		return this->m_size;
	}
	
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::size_type List<T, Allocator>::max_size() const noexcept
	{
		return std::numeric_limits<size_t>::max() / sizeof(T);
	}
	
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::reserve(size_type new_cap)
	{
		if(new_cap > this->max_size) [[likely]]
		{
			this->reallocate<false>(new_cap);
		}
	}
	
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::size_type List<T, Allocator>::capacity() const noexcept
	{
		return this->m_capacity;
	}
	
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::shrink_to_fit()
	{
		this->reallocate<false>(this->m_size);
	}



	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::clear() noexcept
	{
		Allocator alloc;
		alloc.deallocate(this->m_data, this->m_size);
		this->m_data = nullptr;
		this->m_size = 0;
		this->m_capacity = 0;
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::iterator List<T, Allocator>::insert(const_iterator pos, const T& value)
	{
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		this->m_data[pos] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::iterator List<T, Allocator>::insert(const_iterator pos, T&& value)
	{
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		this->m_data[pos] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::iterator List<T, Allocator>::insert(const_iterator pos, size_type count, const T& value)
	{
		if(this->m_size + count >= this->m_capacity) [[unlikely]]
		{
			if(count > this->size_t)
			{
				this->reallocate<false>(this->m_size + count);
			}
			else
			{
				this->reallocate<false>(this->m_size * 2);
			}
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + count);
		std::fill(this->m_data + pos, this->m_data + pos + count, value);
		this->m_size += count;
	}
	template<typename T, typename Allocator>
	template<typename InputIt>
	constexpr List<T, Allocator>::iterator List<T, Allocator>::insert(const_iterator pos, InputIt first, InputIt last)
	{
		size_t count = std::distance(first, last);

		if(this->m_size + count >= this->m_capacity) [[unlikely]]
		{
			if(count > this->size_t)
			{
				this->reallocate<false>(this->m_size + count);
			}
			else
			{
				this->reallocate<false>(this->m_size * 2);
			}
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + count);
		std::copy(first, last, this->m_data + pos);
		this->m_size += count;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::iterator List<T, Allocator>::insert(const_iterator pos, std::initializer_list<T> ilist)
	{
		size_t count = ilist.size();

		if(this->m_size + count >= this->m_capacity) [[unlikely]]
		{
			if(count > this->size_t)
			{
				this->reallocate<false>(this->m_size + count);
			}
			else
			{
				this->reallocate<false>(this->m_size * 2);
			}
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + count);
		std::copy(ilist.begin(), ilist.end(), this->m_data + pos);
		this->m_size += count;
	}
	
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr List<T, Allocator>::iterator List<T, Allocator>::emplace(const_iterator pos, Args&&... args)
	{
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		*pos = T(args...);
		return pos;
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::iterator List<T, Allocator>::erase(const_iterator pos)
	{
		std::copy(this->m_data + pos + 1, this->m_data + this->m_size, this->m_data + pos);
		this->m_size--;
		return iterator(this->m_data + pos);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::iterator List<T, Allocator>::erase(const_iterator first, const_iterator last)
	{
		size_t count = std::distance(first, last);
		std::copy(this->m_data + last, this->m_data + this->m_size, first);
		this->m_size -= count;
		return first;
	}
	
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::push_back(const T& value)
	{
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::push_back(T&& value)
	{
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}

	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void List<T, Allocator>::emplace_back(Args&&... args)
	{
		if(this->m_size == this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		this->m_data[this->m_size] = value_type(args...);
		this->m_size++;
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::pop_back()
	{
		this->m_size--;
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::resize(size_type count)
	{
		if(count < this->m_size)
		{
			this->reallocate<true>(count);
			this->m_size = count;
		}
		else
		{
			this->reallocate<false>(count);
			for(size_t i = this->m_size; i < count; i++)
			{
				this->m_data[i] = value_type();
			}

			this->m_size = count;
		}
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::resize(size_type count, const value_type& value)
	{
		if(count < this->m_size)
		{
			this->reallocate<true>(count);
			this->m_size = count;
		}
		else
		{
			this->reallocate<false>(count);
			for(size_t i = this->m_size; i < count; i++)
			{
				this->m_data[i] = value;
			}
			this->m_size = count;
		}
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::swap(List& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value || std::allocator_traits<Allocator>::is_always_equal::value)
	{
		std::swap(this->m_data, other.m_data);
		std::swap(this->m_size, other.m_size);
		std::swap(this->m_capacity, other.m_capacity);
	}

	template<typename T, typename Allocator>
	template<bool shouldShrink>
	constexpr void List<T, Allocator>::reallocate(const size_type& newCapacity)
	{
		Allocator alloc;
		T* newDataLocation = alloc.allocate(newCapacity);
		T* oldDataLocation = this->m_data;
		if constexpr(shouldShrink)
		{
			std::copy(oldDataLocation, oldDataLocation + newCapacity, newDataLocation);
		}
		else
		{
			std::copy(oldDataLocation, oldDataLocation + this->m_size, newDataLocation);
		}
		this->m_data = newDataLocation;
		this->m_capacity = newCapacity;
		if(oldDataLocation != nullptr) [[unlikely]]
		{
			alloc.deallocate(oldDataLocation, this->m_size);
		}
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::allocate(const size_type& newCapacity)
	{
		Allocator alloc;
		if(this->m_data != nullptr)
		{
			alloc.deallocate(this->m_data, this->m_size);
		}
		this->m_data = alloc.allocate(newCapacity);
	}

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs)
	{
		return (lhs.data() == rhs.data()) && (lhs.size() == rhs.size()) && (lhs.capacity() == rhs.capacity());
	}
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator!=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs)
	{
		return !((lhs.data() == rhs.data()) && (lhs.size() == rhs.size()) && (lhs.capacity() == rhs.capacity()));
	}
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator<(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs)
	{
		return lhs.size() < rhs.size();
	}
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator<=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs)
	{
		return lhs.size() <= rhs.size();
	}
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator>(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs)
	{
		return lhs.size() > rhs.size();
	}
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator>=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs)
	{
		return lhs.size() >= rhs.size();
	}
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr auto operator<=>(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs)
	{
		return lhs.size() <=> rhs.size();
	}



	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::TS_List() noexcept(noexcept(Allocator()))
	: m_data(nullptr), m_size(0), m_capacity(0), m_lock()
	{}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::TS_List(const Allocator& alloc) noexcept
	: m_data(nullptr), m_size(0), m_capacity(0), m_lock()
	{}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::TS_List(size_type count, const T& value, const Allocator& alloc)
	: m_data(nullptr), m_size(count), m_capacity(count), m_lock()
	{
		this->m_data = alloc.allocate(count);
		std::fill(this->m_data, this->m_data + this->m_size, value);
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::TS_List(size_type count, const Allocator& alloc)
	: m_data(nullptr), m_size(count), m_capacity(count), m_lock()
	{
		this->m_data = alloc.allocate(count);
		std::fill(this->m_data, this->m_data + this->m_size, value_type());
	}
	template<typename T, typename Allocator>
	template<typename InputIt>
	constexpr TS_List<T, Allocator>::TS_List(InputIt first, InputIt last, const Allocator& alloc)
	{
		auto size = std::distance(first, last);
		this->m_data = alloc.allocate(size);
		std::copy(first, last, this->m_data);
		this->m_size = size;
		this->m_capacity = size;
		this->m_lock();
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::TS_List(const TS_List& other)
	: m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity), m_lock()
	{
		Allocator alloc;
		this->m_data = alloc.allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::TS_List(const TS_List& other, const Allocator& alloc)
	: m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity), m_lock()
	{
		this->m_data = alloc.allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::TS_List(TS_List&& other) noexcept
	: m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity), m_lock()
	{
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::TS_List(TS_List&& other, const Allocator& alloc)
	: m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity), m_lock()
	{
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::TS_List(std::initializer_list<T> init, const Allocator& alloc)
	: m_data(nullptr), m_size(init.size()), m_capacity(init.size()), m_lock()
	{
		this->m_data = alloc.allocate(init.size());
		std::copy(init.begin(), init.end(), this->m_size);
	}

	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::~TS_List()
	{
		this->m_lock.writeLock();
		this->m_lock.writeUnlock();

		if(this->m_data != nullptr)
		{
			Allocator alloc;
			alloc.deallocate(this->m_data, this->m_capacity);
		}
	}

	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>& TS_List<T, Allocator>::operator=(const TS_List& other)
	{
		WriteLockGuard(&(this->m_lock));
		
		this->allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>& TS_List<T, Allocator>::operator=(TS_List&& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<Allocator>::is_always_equal::value)
	{
		WriteLockGuard(&(this->m_lock));
		
		this->m_data = other.m_data;
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>& TS_List<T, Allocator>::operator=(std::initializer_list<T> ilist)
	{
		WriteLockGuard(&(this->m_lock));
		
		auto size = ilist.size();
		this->allocate(size);
		std::copy(ilist.begin(), ilist.end(), this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}

	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::assign(size_type count, const T& value)
	{
		WriteLockGuard(&(this->m_lock));
		
		this->allocate(count);
		std::fill(this->m_data, this->m_data + count, value);
		this->m_size = count;
		this->m_capacity = count;
	}
	template<typename T, typename Allocator>
	template<typename InputIt>
	constexpr void TS_List<T, Allocator>::assign(InputIt first, InputIt last)
	{
		WriteLockGuard(&(this->m_lock));
		
		auto size = std::distance(first, last);
		this->allocate(size);
		std::copy(first, last, this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}
	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::assign(std::initializer_list<T> ilist)
	{
		WriteLockGuard(&(this->m_lock));
		
		auto size = ilist.size();
		this->allocate(size);
		std::copy(ilist.begin(), ilist.end(), this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}

	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::reference TS_List<T, Allocator>::at(size_type pos)
	{
		ReadLockGuard(&(this->m_lock));
		
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::TS_List index out of range");
		}
	}

	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::reference TS_List<T, Allocator>::operator[](size_type pos)
	{
		ReadLockGuard(&(this->m_lock));
		
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::TS_List index out of range");
		}
	}

	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::reference TS_List<T, Allocator>::front()
	{
		ReadLockGuard(&(this->m_lock));
		
		return this->m_data[0];
	}
	
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::reference TS_List<T, Allocator>::back()
	{
		ReadLockGuard(&(this->m_lock));
		
		return this->m_data[this->m_size-1];
	}
	
	template<typename T, typename Allocator>
	constexpr T* TS_List<T, Allocator>::data() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return this->m_data;
	}
	
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::iterator TS_List<T, Allocator>::begin() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return iterator(this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::const_iterator TS_List<T, Allocator>::cbegin() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return const_iterator(this->m_data);
	}

	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::iterator TS_List<T, Allocator>::end() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return iterator(this->m_data + this->m_size);
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::const_iterator TS_List<T, Allocator>::cend() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return const_iterator(this->m_data + this->m_size);
	}

	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::reverse_iterator TS_List<T, Allocator>::rbegin() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return reverse_iterator(this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::const_reverse_iterator TS_List<T, Allocator>::crbegin() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return const_reverse_iterator(this->m_data);
	}

	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::reverse_iterator TS_List<T, Allocator>::rend() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return reverse_iterator(this->m_data + this->m_size);
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::const_reverse_iterator TS_List<T, Allocator>::crend() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return const_reverse_iterator(this->m_data + this->m_size);
	}


	template<typename T, typename Allocator>
	[[nodiscard]] constexpr bool TS_List<T, Allocator>::empty() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return (this->m_size == 0);
	}
	
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::size_type TS_List<T, Allocator>::size() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return this->m_size;
	}
	
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::size_type TS_List<T, Allocator>::max_size() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return std::numeric_limits<size_t>::max() / sizeof(T);
	}
	
	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::reserve(size_type new_cap)
	{
		WriteLockGuard(&(this->m_lock));
		
		if(new_cap > this->max_size) [[likely]]
		{
			this->reallocate<false>(new_cap);
		}
	}
	
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::size_type TS_List<T, Allocator>::capacity() noexcept
	{
		ReadLockGuard(&(this->m_lock));
		
		return this->m_capacity;
	}
	
	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::shrink_to_fit()
	{
		WriteLockGuard(&(this->m_lock));
		
		this->reallocate<false>(this->m_size);
	}



	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::clear() noexcept
	{
		WriteLockGuard(&(this->m_lock));
		
		Allocator alloc;
		alloc.deallocate(this->m_data, this->m_size);
		this->m_data = nullptr;
		this->m_size = 0;
		this->m_capacity = 0;
	}

	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::iterator TS_List<T, Allocator>::insert(const_iterator pos, const T& value)
	{
		WriteLockGuard(&(this->m_lock));
		
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		this->m_data[pos] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::iterator TS_List<T, Allocator>::insert(const_iterator pos, T&& value)
	{
		WriteLockGuard(&(this->m_lock));
		
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		this->m_data[pos] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::iterator TS_List<T, Allocator>::insert(const_iterator pos, size_type count, const T& value)
	{
		WriteLockGuard(&(this->m_lock));
		
		if(this->m_size + count >= this->m_capacity) [[unlikely]]
		{
			if(count > this->size_t)
			{
				this->reallocate<false>(this->m_size + count);
			}
			else
			{
				this->reallocate<false>(this->m_size * 2);
			}
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + count);
		std::fill(this->m_data + pos, this->m_data + pos + count, value);
		this->m_size += count;
	}
	template<typename T, typename Allocator>
	template<typename InputIt>
	constexpr TS_List<T, Allocator>::iterator TS_List<T, Allocator>::insert(const_iterator pos, InputIt first, InputIt last)
	{
		WriteLockGuard(&(this->m_lock));
		
		size_t count = std::distance(first, last);

		if(this->m_size + count >= this->m_capacity) [[unlikely]]
		{
			if(count > this->size_t)
			{
				this->reallocate<false>(this->m_size + count);
			}
			else
			{
				this->reallocate<false>(this->m_size * 2);
			}
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + count);
		std::copy(first, last, this->m_data + pos);
		this->m_size += count;
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::iterator TS_List<T, Allocator>::insert(const_iterator pos, std::initializer_list<T> ilist)
	{
		WriteLockGuard(&(this->m_lock));
		
		size_t count = ilist.size();

		if(this->m_size + count >= this->m_capacity) [[unlikely]]
		{
			if(count > this->size_t)
			{
				this->reallocate<false>(this->m_size + count);
			}
			else
			{
				this->reallocate<false>(this->m_size * 2);
			}
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + count);
		std::copy(ilist.begin(), ilist.end(), this->m_data + pos);
		this->m_size += count;
	}
	
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr TS_List<T, Allocator>::iterator TS_List<T, Allocator>::emplace(const_iterator pos, Args&&... args)
	{
		WriteLockGuard(&(this->m_lock));
		
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		*pos = T(args...);
		return pos;
	}

	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::iterator TS_List<T, Allocator>::erase(const_iterator pos)
	{
		WriteLockGuard(&(this->m_lock));
		
		std::copy(this->m_data + pos + 1, this->m_data + this->m_size, this->m_data + pos);
		this->m_size--;
		return iterator(this->m_data + pos);
	}
	template<typename T, typename Allocator>
	constexpr TS_List<T, Allocator>::iterator TS_List<T, Allocator>::erase(const_iterator first, const_iterator last)
	{
		WriteLockGuard(&(this->m_lock));
		
		size_t count = std::distance(first, last);
		std::copy(this->m_data + last, this->m_data + this->m_size, first);
		this->m_size -= count;
		return first;
	}
	
	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::push_back(const T& value)
	{
		WriteLockGuard(&(this->m_lock));
		
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::push_back(T&& value)
	{
		WriteLockGuard(&(this->m_lock));
		
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}

	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void TS_List<T, Allocator>::emplace_back(Args&&... args)
	{
		WriteLockGuard(&(this->m_lock));
		
		if(this->m_size == this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		this->m_data[this->m_size] = value_type(args...);
		this->m_size++;
	}

	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::pop_back()
	{
		WriteLockGuard(&(this->m_lock));
		
		this->m_size--;
	}

	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::resize(size_type count)
	{
		WriteLockGuard(&(this->m_lock));
		
		if(count < this->m_size)
		{
			this->reallocate<true>(count);
			this->m_size = count;
		}
		else
		{
			this->reallocate<false>(count);
			for(size_t i = this->m_size; i < count; i++)
			{
				this->m_data[i] = value_type();
			}

			this->m_size = count;
		}
	}
	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::resize(size_type count, const value_type& value)
	{
		WriteLockGuard(&(this->m_lock));
		
		if(count < this->m_size)
		{
			this->reallocate<true>(count);
			this->m_size = count;
		}
		else
		{
			this->reallocate<false>(count);
			for(size_t i = this->m_size; i < count; i++)
			{
				this->m_data[i] = value;
			}
			this->m_size = count;
		}
	}

	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::swap(TS_List& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value || std::allocator_traits<Allocator>::is_always_equal::value)
	{
		WriteLockGuard(&(this->m_lock));
		
		std::swap(this->m_data, other.m_data);
		std::swap(this->m_size, other.m_size);
		std::swap(this->m_capacity, other.m_capacity);
	}

	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::flush()
	{
		this->m_lock.writeLock();
		this->m_lock.writeUnlock();
	}

	template<typename T, typename Allocator>
	template<bool shouldShrink>
	constexpr void TS_List<T, Allocator>::reallocate(const size_type& newCapacity)
	{
		Allocator alloc;
		T* newDataLocation = alloc.allocate(newCapacity);
		T* oldDataLocation = this->m_data;
		if constexpr(shouldShrink)
		{
			std::copy(oldDataLocation, oldDataLocation + newCapacity, newDataLocation);
		}
		else
		{
			std::copy(oldDataLocation, oldDataLocation + this->m_size, newDataLocation);
		}
		this->m_data = newDataLocation;
		this->m_capacity = newCapacity;
		if(oldDataLocation != nullptr) [[unlikely]]
		{
			alloc.deallocate(oldDataLocation, this->m_size);
		}
	}
	template<typename T, typename Allocator>
	constexpr void TS_List<T, Allocator>::allocate(const size_type& newCapacity)
	{
		Allocator alloc;
		if(this->m_data != nullptr)
		{
			alloc.deallocate(this->m_data, this->m_size);
		}
		this->m_data = alloc.allocate(newCapacity);
	}

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs)
	{
		return (lhs.data() == rhs.data()) && (lhs.size() == rhs.size()) && (lhs.capacity() == rhs.capacity());
	}
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator!=(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs)
	{
		return !((lhs.data() == rhs.data()) && (lhs.size() == rhs.size()) && (lhs.capacity() == rhs.capacity()));
	}
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator<(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs)
	{
		return lhs.size() < rhs.size();
	}
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator<=(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs)
	{
		return lhs.size() <= rhs.size();
	}
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator>(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs)
	{
		return lhs.size() > rhs.size();
	}
	template<typename T, typename Allocator = std::allocator<T>>
	bool operator>=(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs)
	{
		return lhs.size() >= rhs.size();
	}
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr auto operator<=>(TS_List<T, Allocator>& lhs, TS_List<T, Allocator>& rhs)
	{
		return lhs.size() <=> rhs.size();
	}
}