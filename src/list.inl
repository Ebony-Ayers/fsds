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
	: m_data(nullptr), m_size(count), m_capacity(count * 2)
	{
		this->m_data = alloc.allocate(count * 2);
		std::fill(this->m_data, this->m_data + this->m_size, value);
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::basic_list(size_type count, const Allocator& alloc)
	: m_data(nullptr), m_size(count), m_capacity(count * 2)
	{
		this->m_data = alloc.allocate(count * 2);
		std::fill(this->m_data, this->m_data + this->m_size, value_type());
	}




	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::~basic_list()
	{
		Allocator alloc;
		alloc.deallocate(this->m_data, this->m_capacity);
	}




	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::reference basic_list<T, Allocator, ThreadSycronisationObject>::at(size_type pos)
	{
		if(pos < m_size)
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range();
		}
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::const_reference basic_list<T, Allocator, ThreadSycronisationObject>::at(size_type pos) const
	{
		if(pos < m_size)
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range();
		}
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::reference basic_list<T, Allocator, ThreadSycronisationObject>::operator[](size_type pos)
	{
		if(pos < m_size)
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range();
		}
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr basic_list<T, Allocator, ThreadSycronisationObject>::const_reference basic_list<T, Allocator, ThreadSycronisationObject>::operator[](size_type pos) const
	{
		if(pos < m_size)
		{
			return this->m_data[pos];
		}
		else
		{
			throw std::out_of_range();
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
		if(new_cap > this->max_size)
		{
			Allocator alloc;
			auto newDataLocation = alloc.allocate(new_cap);
			auto oldDataLocation = this->m_data;
			std::copy(oldDataLocation, oldDataLocation + this->m_size, newDataLocation);
			this->m_data = newDataLocation;
			this->m_capacity = new_cap;
			alloc.deallocate(oldDataLocation, this->m_size);
			
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
		Allocator alloc;
		auto newDataLocation = alloc.allocate(this->m_size);
		auto oldDataLocation = this->m_data;
		std::copy(oldDataLocation, oldDataLocation + this->m_size, newDataLocation);
		this->m_data = newDataLocation;
		this->m_capacity = this->m_size;
		alloc.deallocate(oldDataLocation, this->m_size);
	}



	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::clear() noexcept
	{
		Allocator alloc;
		alloc.deallocate(this->m_data, this->m_size);
		this->m_size = 0;
		this->m_size = 0;
	}
	
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::push_back(const T& value)
	{
		if(this->m_size == this->m_capacity)
		{
			Allocator alloc;
			auto newDataLocation = alloc.allocate(this->m_size * 2);
			auto oldDataLocation = this->m_data;
			std::copy(oldDataLocation, oldDataLocation + this->m_size, newDataLocation);
			this->m_data = newDataLocation;
			this->m_capacity = this->m_size * 2;
			alloc.deallocate(oldDataLocation, this->m_size);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::push_back(T&& value)
	{
		if(this->m_size == this->m_capacity)
		{
			Allocator alloc;
			auto newDataLocation = alloc.allocate(this->m_size * 2);
			auto oldDataLocation = this->m_data;
			std::copy(oldDataLocation, oldDataLocation + this->m_size, newDataLocation);
			this->m_data = newDataLocation;
			this->m_capacity = this->m_size * 2;
			alloc.deallocate(oldDataLocation, this->m_size);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	template<typename... Args>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::emplace_back(Args&&... args)
	{
		if(this->m_size == this->m_capacity)
		{
			Allocator alloc;
			auto newDataLocation = alloc.allocate(this->m_size * 2);
			auto oldDataLocation = this->m_data;
			std::copy(oldDataLocation, oldDataLocation + this->m_size, newDataLocation);
			this->m_data = newDataLocation;
			this->m_capacity = this->m_size * 2;
			alloc.deallocate(oldDataLocation, this->m_size);
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
			Allocator alloc;
			auto newDataLocation = alloc.allocate(count);
			auto oldDataLocation = this->m_data;
			std::copy(oldDataLocation, oldDataLocation + count, newDataLocation);
			this->m_data = newDataLocation;
			this->m_capacity = count;
			alloc.deallocate(oldDataLocation, this->m_size);
			this->m_size = count;
		}
		else
		{
			Allocator alloc;
			auto newDataLocation = alloc.allocate(count);
			auto oldDataLocation = this->m_data;
			std::copy(oldDataLocation, oldDataLocation + this->m_size, newDataLocation);
			this->m_data = newDataLocation;
			this->m_capacity = count;
			alloc.deallocate(oldDataLocation, this->m_size);

			for(size_t i = this->m_size; i < count; i++)
			{
				this->m_data[i] = value_type();
			}
		}
	}
	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::resize(size_type count, const value_type& value)
	{
		if(count < this->m_size)
		{
			Allocator alloc;
			auto newDataLocation = alloc.allocate(count);
			auto oldDataLocation = this->m_data;
			std::copy(oldDataLocation, oldDataLocation + count, newDataLocation);
			this->m_data = newDataLocation;
			this->m_capacity = count;
			alloc.deallocate(oldDataLocation, this->m_size);
			this->m_size = count;
		}
		else
		{
			Allocator alloc;
			auto newDataLocation = alloc.allocate(count);
			auto oldDataLocation = this->m_data;
			std::copy(oldDataLocation, oldDataLocation + this->m_size, newDataLocation);
			this->m_data = newDataLocation;
			this->m_capacity = count;
			alloc.deallocate(oldDataLocation, this->m_size);

			for(size_t i = this->m_size; i < count; i++)
			{
				this->m_data[i] = value;
			}
		}
	}

	template<typename T, typename Allocator, typename ThreadSycronisationObject>
	constexpr void basic_list<T, Allocator, ThreadSycronisationObject>::swap(basic_list& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value || std::allocator_traits<Allocator>::is_always_equal::value)
	{
		std::swap(this->m_data, other.m_data);
		std::swap(this->m_size, other.m_size);
		std::swap(this->m_capacity, other.m_capacity);
	}
}