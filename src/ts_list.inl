namespace fsds
{
	template<typename T, typename Allocator>
	constexpr ts_List<T, Allocator>::ts_List() noexcept(noexcept(Allocator()))
	: m_data(nullptr), m_size(0), m_capacity(4), m_lock()
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		Allocator alloc;
		m_data = alloc.allocate(4);
	}
	template<typename T, typename Allocator>
	constexpr ts_List<T, Allocator>::ts_List(std::initializer_list<T> init, const Allocator& alloc)
	: m_data(nullptr), m_size(init.size()), m_capacity(init.size()), m_lock()
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		m_data = alloc.allocate(init.size());
		std::copy(init.begin(), init.end(), this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr ts_List<T, Allocator>::~ts_List()
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		Allocator alloc;
		alloc.deallocate(this->m_data, this->m_capacity);
	}

	template<typename T, typename Allocator>
	constexpr Allocator ts_List<T, Allocator>::getAllocator() const noexcept
	{
		Allocator alloc;
		return alloc;
	}

	template<typename T, typename Allocator>
	constexpr T* ts_List<T, Allocator>::operator[](size_t pos)
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		return this->m_data + pos;
	}

	template<typename T, typename Allocator>
	constexpr T* ts_List<T, Allocator>::front()
	{
		ts_ListReadLockGuard(&this->m_lock);
		
		return this->m_data;
	}
	template<typename T, typename Allocator>
	constexpr T* ts_List<T, Allocator>::back()
	{
		ts_ListReadLockGuard(&this->m_lock);
		
		return this->m_data + (this->m_size - 1);
	}

	template<typename T, typename Allocator>
	constexpr T* ts_List<T, Allocator>::data()
	{
		ts_ListReadLockGuard(&this->m_lock);
		
		return this->m_data;
	}

	template<typename T, typename Allocator>
	[[nodiscard]] constexpr bool ts_List<T, Allocator>::isEmpty() noexcept
	{
		ts_ListReadLockGuard(&this->m_lock);
		
		return this->m_size == 0;
	}
	template<typename T, typename Allocator>
	constexpr size_t ts_List<T, Allocator>::size() noexcept
	{
		ts_ListReadLockGuard(&this->m_lock);
		
		return this->m_size;
	}
	template<typename T, typename Allocator>
	constexpr size_t ts_List<T, Allocator>::maxSize() noexcept
	{
		ts_ListReadLockGuard(&this->m_lock);
		
		return std::numeric_limits<size_t>::max() / sizeof(T);
	}
	template<typename T, typename Allocator>
	constexpr size_t ts_List<T, Allocator>::capacity() noexcept
	{
		ts_ListReadLockGuard(&this->m_lock);
		
		return this->m_capacity;
	}

	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::reserve(size_t newCap)
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		if(newCap > this->m_capacity)
		{
			this->reallocate(newCap);
		}
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::clear()
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		Allocator alloc;
		alloc.deallocate(this->m_data, this->m_capacity);
		this->m_data = alloc.allocate(4);
		this->m_size = 0;
		this->m_capacity = 4;
	}
	
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::append(const T& value)
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void ts_List<T, Allocator>::appendConstruct(Args&&... args)
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		this->m_data[this->m_size] = T(args...);
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::prepend(const T& value)
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		std::copy(this->m_data, this->m_data + this->m_size, this->m_data + 1);
		this->m_data[0] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void ts_List<T, Allocator>::prependConstruct(Args&&... args)
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		std::copy(this->m_data, this->m_data + this->m_size, this->m_data + 1);
		this->m_data[0] = T(args...);
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::insert(size_t pos, const T& value)
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + 1);
		this->m_data[pos] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void ts_List<T, Allocator>::insertConstruct(size_t pos, Args&&... args)
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + 1);
		this->m_data[pos] = T(args...);
		this->m_size++;
	}

	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::remove(size_t pos)
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		this->m_data[pos].~T();
		std::copy(this->m_data + pos + 1, this->m_data + this->m_size, this->m_data + pos);
		this->m_size--;
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::removeBack()
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		this->m_data[this->m_size - 1].~T();
		this->m_size--;
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::removeFront()
	{
		ts_ListWriteLockGuard(&this->m_lock);
		
		this->m_data[0].~T();
		std::copy(this->m_data + 1, this->m_data + this->m_size, this->m_data);
		this->m_size--;
	}

	template<typename T, typename Allocator>
	void ts_List<T, Allocator>::reallocate(size_t newSize)
	{
		Allocator alloc;
		T* oldData = this->m_data;
		this->m_data = alloc.allocate(newSize);
		std::copy(oldData, oldData + this->m_size, this->m_data);
		alloc.deallocate(oldData, this->m_capacity);
		this->m_capacity = newSize;
	}

}