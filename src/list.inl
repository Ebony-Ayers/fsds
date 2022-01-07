namespace fsds
{
	template<typename T>
	BasicListIterator<T>::BasicListIterator(pointer ptr)
	: m_ptr(ptr)
	{}
	
	template<typename T>
	BasicListIterator<T>::reference BasicListIterator<T>::operator*() const
	{
		return *(this->m_ptr);
	}
	template<typename T>
	BasicListIterator<T>::pointer BasicListIterator<T>::operator->()
	{
		return this->m_ptr;
	}
	template<typename T>
	BasicListIterator<T>::reference BasicListIterator<T>::operator[](size_type pos) const
	{
		return this->m_ptr[pos];
	}

	template<typename T>
	BasicListIterator<T>& BasicListIterator<T>::operator++()
	{
		this->m_ptr++;
		return this;
	}
	template<typename T>
	BasicListIterator<T>& BasicListIterator<T>::operator++(int)
	{
		auto previous = *this;
		this->m_ptr++;
		return previous;
	}
	template<typename T>
	BasicListIterator<T>& BasicListIterator<T>::operator--()
	{
		this->m_ptr--;
		return this;
	}
	template<typename T>
	BasicListIterator<T>& BasicListIterator<T>::operator--(int)
	{
		auto previous = *this;
		this->m_ptr--;
		return previous;
	}

	template<typename T>
	BasicListIterator<T>& BasicListIterator<T>::operator+=(size_type value)
	{
		this->m_ptr += value;
		return *this;
	}
	template<typename T>
	BasicListIterator<T>& BasicListIterator<T>::operator-=(size_type value)
	{
		this->m_ptr -= value;
		return *this;
	}

	template<typename T>
	bool operator==(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs)
	{
		return lhs.m_ptr == rhs.m_ptr;
	}
	template<typename T>
	bool operator!=(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs)
	{
		return lhs.m_ptr != rhs.m_ptr;
	}
	template<typename T>
	bool operator<(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs)
	{
		return lhs.m_ptr < rhs.m_ptr;
	}
	template<typename T>
	bool operator<=(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs)
	{
		return lhs.m_ptr <= rhs.m_ptr;
	}
	template<typename T>
	bool operator>(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs)
	{
		return lhs.m_ptr > rhs.m_ptr;
	}
	template<typename T>
	bool operator>=(const BasicListIterator<T>& lhs, const BasicListIterator<T>& rhs)
	{
		return lhs.m_ptr >= rhs.m_ptr;
	}

	template<typename T>
	BasicListIterator<T> operator+(const BasicListIterator<T>& lhs, const typename BasicListIterator<T>::size_type& rhs)
	{
		return BasicListIterator<T>(lhs.m_ptr + rhs);
	}
	template<typename T>
	BasicListIterator<T> operator+(const typename BasicListIterator<T>::size_type& lhs, const BasicListIterator<T>& rhs)
	{
		return BasicListIterator<T>(rhs.m_ptr + lhs);
	}
	template<typename T>
	BasicListIterator<T> operator-(const BasicListIterator<T>& lhs, const typename BasicListIterator<T>::size_type& rhs)
	{
		return BasicListIterator<T>(lhs.m_ptr - rhs);
	}
	template<typename T>
	BasicListIterator<T> operator-(const typename BasicListIterator<T>::size_type& lhs, const BasicListIterator<T>& rhs)
	{
		return BasicListIterator<T>(rhs.m_ptr - lhs);
	}


	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::BasicList() noexcept(noexcept(Allocator()))
	: m_data(nullptr), m_size(0), m_capacity(0)
	{}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::BasicList(const Allocator& alloc) noexcept
	: m_data(nullptr), m_size(0), m_capacity(0)
	{}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::BasicList(size_type count, const T& value, const Allocator& alloc)
	: m_data(nullptr), m_size(count), m_capacity(count)
	{
		this->m_data = alloc.allocate(count);
		std::fill(this->m_data, this->m_data + this->m_size, value);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::BasicList(size_type count, const Allocator& alloc)
	: m_data(nullptr), m_size(count), m_capacity(count)
	{
		this->m_data = alloc.allocate(count);
		std::fill(this->m_data, this->m_data + this->m_size, value_type());
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	template<typename InputIt>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::BasicList(InputIt first, InputIt last, const Allocator& alloc)
	{
		auto size = std::distance(first, last);
		this->m_data = alloc.allocate(size);
		std::copy(first, last, this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::BasicList(const BasicList& other)
	: m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		Allocator alloc;
		this->m_data = alloc.allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::BasicList(const BasicList& other, const Allocator& alloc)
	: m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		this->m_data = alloc.allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::BasicList(BasicList&& other) noexcept
	: m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::BasicList(BasicList&& other, const Allocator& alloc)
	: m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::BasicList(std::initializer_list<T> init, const Allocator& alloc)
	: m_data(nullptr), m_size(init.size()), m_capacity(init.size())
	{
		this->m_data = alloc.allocate(init.size());
		std::copy(init.begin(), init.end(), this->m_size);
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::~BasicList()
	{
		if(this->m_data != nullptr)
		{
			Allocator alloc;
			alloc.deallocate(this->m_data, this->m_capacity);
		}
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>& BasicList<T, Allocator, ThreadSycronisationObject>::operator=(const BasicList& other)
	{
		this->allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>& BasicList<T, Allocator, ThreadSycronisationObject>::operator=(BasicList&& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<Allocator>::is_always_equal::value)
	{
		this->m_data = other.m_data;
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>& BasicList<T, Allocator, ThreadSycronisationObject>::operator=(std::initializer_list<T> ilist)
	{
		auto size = ilist.size();
		this->allocate(size);
		std::copy(ilist.begin(), ilist.end(), this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::assign(size_type count, const T& value)
	{
		this->allocate(count);
		std::fill(this->m_data, this->m_data + count, value);
		this->m_size = count;
		this->m_capacity = count;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	template<typename InputIt>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::assign(InputIt first, InputIt last)
	{
		auto size = std::distance(first, last);
		this->allocate(size);
		std::copy(first, last, this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::assign(std::initializer_list<T> ilist)
	{
		auto size = ilist.size();
		this->allocate(size);
		std::copy(ilist.begin(), ilist.end(), this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::reference BasicList<T, Allocator, ThreadSycronisationObject>::at(size_type pos)
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::BasicList index out of range");
		}
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_reference BasicList<T, Allocator, ThreadSycronisationObject>::at(size_type pos) const
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::BasicList index out of range");
		}
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::reference BasicList<T, Allocator, ThreadSycronisationObject>::operator[](size_type pos)
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::BasicList index out of range");
		}
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_reference BasicList<T, Allocator, ThreadSycronisationObject>::operator[](size_type pos) const
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::BasicList index out of range");
		}
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::reference BasicList<T, Allocator, ThreadSycronisationObject>::front()
	{
		return this->m_data[0];
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_reference BasicList<T, Allocator, ThreadSycronisationObject>::front() const
	{
		return this->m_data[0];
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::reference BasicList<T, Allocator, ThreadSycronisationObject>::back()
	{
		return this->m_data[this->m_size-1];
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_reference BasicList<T, Allocator, ThreadSycronisationObject>::back() const
	{
		return this->m_data[this->m_size-1];
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr T* BasicList<T, Allocator, ThreadSycronisationObject>::data() noexcept
	{
		return this->m_data;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr const T* BasicList<T, Allocator, ThreadSycronisationObject>::data() const noexcept
	{
		return this->m_data;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::iterator BasicList<T, Allocator, ThreadSycronisationObject>::begin() noexcept
	{
		return iterator(this->m_data);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_iterator BasicList<T, Allocator, ThreadSycronisationObject>::begin() const noexcept
	{
		return const_iterator(this->m_data);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_iterator BasicList<T, Allocator, ThreadSycronisationObject>::cbegin() const noexcept
	{
		return const_iterator(this->m_data);
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::iterator BasicList<T, Allocator, ThreadSycronisationObject>::end() noexcept
	{
		return iterator(this->m_data + this->m_size);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_iterator BasicList<T, Allocator, ThreadSycronisationObject>::end() const noexcept
	{
		return const_iterator(this->m_data + this->m_size);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_iterator BasicList<T, Allocator, ThreadSycronisationObject>::cend() const noexcept
	{
		return const_iterator(this->m_data + this->m_size);
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::reverse_iterator BasicList<T, Allocator, ThreadSycronisationObject>::rbegin() noexcept
	{
		return reverse_iterator(this->m_data);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_reverse_iterator BasicList<T, Allocator, ThreadSycronisationObject>::rbegin() const noexcept
	{
		return const_reverse_iterator(this->m_data);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_reverse_iterator BasicList<T, Allocator, ThreadSycronisationObject>::crbegin() const noexcept
	{
		return const_reverse_iterator(this->m_data);
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::reverse_iterator BasicList<T, Allocator, ThreadSycronisationObject>::rend() noexcept
	{
		return reverse_iterator(this->m_data + this->m_size);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_reverse_iterator BasicList<T, Allocator, ThreadSycronisationObject>::rend() const noexcept
	{
		return const_reverse_iterator(this->m_data + this->m_size);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::const_reverse_iterator BasicList<T, Allocator, ThreadSycronisationObject>::crend() const noexcept
	{
		return const_reverse_iterator(this->m_data + this->m_size);
	}


	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	[[nodiscard]] constexpr bool BasicList<T, Allocator, ThreadSycronisationObject>::empty() const noexcept
	{
		return (this->m_size == 0);
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::size_type BasicList<T, Allocator, ThreadSycronisationObject>::size() const noexcept
	{
		return this->m_size;
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::size_type BasicList<T, Allocator, ThreadSycronisationObject>::max_size() const noexcept
	{
		return std::numeric_limits<size_t>::max() / sizeof(T);
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::reserve(size_type new_cap)
	{
		if(new_cap > this->max_size) [[likely]]
		{
			this->reallocate(new_cap);
		}
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::size_type BasicList<T, Allocator, ThreadSycronisationObject>::capacity() const noexcept
	{
		return this->m_capacity;
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::shrink_to_fit()
	{
		this->reallocate(this->m_size);
	}



	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::clear() noexcept
	{
		Allocator alloc;
		alloc.deallocate(this->m_data, this->m_size);
		this->m_data = nullptr;
		this->m_size = 0;
		this->m_capacity = 0;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::iterator BasicList<T, Allocator, ThreadSycronisationObject>::insert(const_iterator pos, const T& value)
	{
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		this->m_data[pos] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::iterator BasicList<T, Allocator, ThreadSycronisationObject>::insert(const_iterator pos, T&& value)
	{
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		this->m_data[pos] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::iterator BasicList<T, Allocator, ThreadSycronisationObject>::insert(const_iterator pos, size_type count, const T& value)
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
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	template<typename InputIt>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::iterator BasicList<T, Allocator, ThreadSycronisationObject>::insert(const_iterator pos, InputIt first, InputIt last)
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
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::iterator BasicList<T, Allocator, ThreadSycronisationObject>::insert(const_iterator pos, std::initializer_list<T> ilist)
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
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	template<typename... Args>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::iterator BasicList<T, Allocator, ThreadSycronisationObject>::emplace(const_iterator pos, Args&&... args)
	{
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		*pos = T(args...);
		return pos;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::iterator BasicList<T, Allocator, ThreadSycronisationObject>::erase(const_iterator pos)
	{
		std::copy(this->m_data + pos + 1, this->m_data + this->m_size, this->m_data + pos);
		this->m_size--;
		return iterator(this->m_data + pos);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr BasicList<T, Allocator, ThreadSycronisationObject>::iterator BasicList<T, Allocator, ThreadSycronisationObject>::erase(const_iterator first, const_iterator last)
	{
		size_t count = std::distance(first, last);
		std::copy(this->m_data + last, this->m_data + this->m_size, first);
		this->m_size -= count;
		return first;
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::push_back(const T& value)
	{
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::push_back(T&& value)
	{
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	template<typename... Args>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::emplace_back(Args&&... args)
	{
		if(this->m_size == this->m_capacity) [[unlikely]]
		{
			this->reallocate(this->m_size * 2);
		}
		this->m_data[this->m_size] = value_type(args...);
		this->m_size++;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::pop_back()
	{
		this->m_size--;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::resize(size_type count)
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
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::resize(size_type count, const value_type& value)
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

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::swap(BasicList& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value || std::allocator_traits<Allocator>::is_always_equal::value)
	{
		std::swap(this->m_data, other.m_data);
		std::swap(this->m_size, other.m_size);
		std::swap(this->m_capacity, other.m_capacity);
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	template<bool shouldShrink>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::reallocate(const size_type& newCapacity)
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
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void BasicList<T, Allocator, ThreadSycronisationObject>::allocate(const size_type& newCapacity)
	{
		Allocator alloc;
		if(this->m_data != nullptr)
		{
			alloc.deallocate(this->m_data, this->m_size);
		}
		this->m_data = alloc.allocate(newCapacity);
	}

	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	constexpr bool operator==(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs)
	{
		return (lhs.m_data == rhs.m_data) && (lhs.m_size == rhs.m_size) && (lhs.m_capacity == rhs.m_capacity);
	}
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	bool operator!=(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs)
	{
		return !((lhs.m_data == rhs.m_data) && (lhs.m_size == rhs.m_size) && (lhs.m_capacity == rhs.m_capacity));
	}
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	bool operator<(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs)
	{
		return lhs.m_size < rhs.m_size;
	}
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	bool operator<=(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs)
	{
		return lhs.m_size <= rhs.m_size;
	}
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	bool operator>(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs)
	{
		return lhs.m_size > rhs.m_size;
	}
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	bool operator>=(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs)
	{
		return lhs.m_size >= rhs.m_size;
	}
	template<typename T, typename Allocator = std::allocator<T>, typename ThreadSycronisationObject>
	constexpr auto operator<=>(const BasicList<T, Allocator, ThreadSycronisationObject>& lhs, const BasicList<T, Allocator, ThreadSycronisationObject>& rhs)
	{
		return lhs.m_size <=> rhs.m_size;
	}
}