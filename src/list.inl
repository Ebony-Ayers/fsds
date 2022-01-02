namespace fsds
{
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list() noexcept(noexcept(Allocator()))
	: m_data(nullptr), m_size(0), m_capacity(0)
	{}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list(const Allocator& alloc) noexcept
	: m_data(nullptr), m_size(0), m_capacity(0)
	{}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list(size_type count, const T& value, const Allocator& alloc)
	: m_data(nullptr), m_size(count), m_capacity(count)
	{
		this->m_data = alloc.allocate(count);
		std::fill(this->m_data, this->m_data + this->m_size, value);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list(size_type count, const Allocator& alloc)
	: m_data(nullptr), m_size(count), m_capacity(count)
	{
		this->m_data = alloc.allocate(count);
		std::fill(this->m_data, this->m_data + this->m_size, value_type());
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	template<typename InputIt>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list(InputIt first, InputIt last, const Allocator& alloc)
	{
		auto size = std::distance(first, last);
		this->m_data = alloc.allocate(size);
		std::copy(first, last, this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list(const basic_list& other)
	: m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		Allocator alloc;
		this->m_data = alloc.allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list(const basic_list& other, const Allocator& alloc)
	: m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		this->m_data = alloc.allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list(basic_list&& other) noexcept
	: m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list(basic_list&& other, const Allocator& alloc)
	: m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
	{
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list(std::initializer_list<T> init, const Allocator& alloc)
	: m_data(nullptr), m_size(init.size()), m_capacity(init.size())
	{
		this->m_data = alloc.allocate(init.size());
		std::copy(init.begin(), init.end(), this->m_size);
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::~basic_list()
	{
		if(this->m_data != nullptr)
		{
			Allocator alloc;
			alloc.deallocate(this->m_data, this->m_capacity);
		}
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>& basic_list<T, Allocator, ThreadSycronisationObject>::operator=(const basic_list& other)
	{
		this->allocate(other.m_size);
		std::copy(other.m_data, other.m_data + other.m_size, this->m_data);
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>& basic_list<T, Allocator, ThreadSycronisationObject>::operator=(basic_list&& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<Allocator>::is_always_equal::value)
	{
		this->m_data = other.m_data;
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>& basic_list<T, Allocator, ThreadSycronisationObject>::operator=(std::initializer_list<T> ilist)
	{
		auto size = ilist.size();
		this->allocate(size);
		std::copy(ilist.begin(), ilist.end(), this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::assign(size_type count, const T& value)
	{
		this->allocate(count);
		std::fill(this->m_data, this->m_data + count, value);
		this->m_size = count;
		this->m_capacity = count;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	template<typename InputIt>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::assign(InputIt first, InputIt last)
	{
		auto size = std::distance(first, last);
		this->allocate(size);
		std::copy(first, last, this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::assign(std::initializer_list<T> ilist)
	{
		auto size = ilist.size();
		this->allocate(size);
		std::copy(ilist.begin(), ilist.end(), this->m_data);
		this->m_size = size;
		this->m_capacity = size;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::reference basic_list<T, Allocator, ThreadSycronisationObject>::at(size_type pos)
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::basic_list index out of range");
		}
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::const_reference basic_list<T, Allocator, ThreadSycronisationObject>::at(size_type pos) const
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::basic_list index out of range");
		}
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::reference basic_list<T, Allocator, ThreadSycronisationObject>::operator[](size_type pos)
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::basic_list index out of range");
		}
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::const_reference basic_list<T, Allocator, ThreadSycronisationObject>::operator[](size_type pos) const
	{
		if(pos < m_size) [[likely]]
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range("fsds::basic_list index out of range");
		}
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::reference basic_list<T, Allocator, ThreadSycronisationObject>::front()
	{
		return this->m_data[0];
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::const_reference basic_list<T, Allocator, ThreadSycronisationObject>::front() const
	{
		return this->m_data[0];
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::reference basic_list<T, Allocator, ThreadSycronisationObject>::back()
	{
		return this->m_data[this->m_size-1];
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::const_reference basic_list<T, Allocator, ThreadSycronisationObject>::back() const
	{
		return this->m_data[this->m_size-1];
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr T* basic_list<T, Allocator, ThreadSycronisationObject>::data() noexcept
	{
		return this->m_data;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr const T* basic_list<T, Allocator, ThreadSycronisationObject>::data() const noexcept
	{
		return this->m_data;
	}




	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	[[nodiscard]] constexpr bool basic_list<T, Allocator, ThreadSycronisationObject>::empty() const noexcept
	{
		return (this->m_size == 0);
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::size_type basic_list<T, Allocator, ThreadSycronisationObject>::size() const noexcept
	{
		return this->m_size;
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::size_type basic_list<T, Allocator, ThreadSycronisationObject>::max_size() const noexcept
	{
		return std::numeric_limits<size_t>::max() / sizeof(T);
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::reserve(size_type new_cap)
	{
		if(new_cap > this->max_size) [[likely]]
		{
			this->reallocate(new_cap);
		}
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::size_type basic_list<T, Allocator, ThreadSycronisationObject>::capacity() const noexcept
	{
		return this->m_capacity;
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::shrink_to_fit()
	{
		this->reallocate(this->m_size);
	}



	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::clear() noexcept
	{
		Allocator alloc;
		alloc.deallocate(this->m_data, this->m_size);
		this->m_data = nullptr;
		this->m_size = 0;
		this->m_capacity = 0;
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::push_back(const T& value)
	{
		if(this->m_size >= this->m_capacity) [[unlikely]]
		{
			this->reallocate<false>(this->m_size * 2);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::push_back(T&& value)
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
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::emplace_back(Args&&... args)
	{
		if(this->m_size == this->m_capacity) [[unlikely]]
		{
			this->reallocate(this->m_size * 2);
		}
		this->m_data[this->m_size] = value_type(args...);
		this->m_size++;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::pop_back()
	{
		this->m_size--;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::resize(size_type count)
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
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::resize(size_type count, const value_type& value)
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
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::swap(basic_list& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value || std::allocator_traits<Allocator>::is_always_equal::value)
	{
		std::swap(this->m_data, other.m_data);
		std::swap(this->m_size, other.m_size);
		std::swap(this->m_capacity, other.m_capacity);
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	template<bool shouldShrink>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::reallocate(const size_type& newCapacity)
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
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::allocate(const size_type& newCapacity)
	{
		Allocator alloc;
		if(this->m_data != nullptr)
		{
			alloc.deallocate(this->m_data, this->m_size);
		}
		this->m_data = alloc.allocate(newCapacity);
	}
}