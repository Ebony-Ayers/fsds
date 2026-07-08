namespace fsds
{
	template<typename T, typename Allocator>
	constexpr SPSCQueue<T, Allocator>::SPSCQueue() noexcept(noexcept(Allocator()))
	: m_data(nullptr), m_frontOffset(0), m_backOffset(0), m_size(0), m_capacity(SPSCQueue<T, Allocator>::sm_baseAllocation)
	{
		Allocator alloc;
		this->m_data = alloc.allocate(SPSCQueue<T, Allocator>::sm_baseAllocation);
	}
	template<typename T, typename Allocator>
	constexpr SPSCQueue<T, Allocator>::SPSCQueue(size_t defaultSize)
	: m_data(nullptr), m_frontOffset(0), m_backOffset(0), m_size(0), m_capacity(defaultSize)
	{
		Allocator alloc;
		this->m_data = alloc.allocate(defaultSize);
	}
	template<typename T, typename Allocator>
	constexpr SPSCQueue<T, Allocator>::SPSCQueue(const SPSCQueue& other)
	{
		this->m_data = nullptr;
		other.deepCopy(*this);
	}
	template<typename T, typename Allocator>
	constexpr SPSCQueue<T, Allocator>::SPSCQueue(SPSCQueue&& other) noexcept
	{
		this->m_data = other.m_data;
		this->m_frontOffset = other.m_frontOffset;
		this->m_backOffset = other.m_backOffset;
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;

		other.m_data = nullptr;
		other.m_frontOffset = 0;
		other.m_backOffset = 0;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator>
	constexpr SPSCQueue<T, Allocator>::~SPSCQueue()
	{
		if(this->m_data != nullptr)
		{
			Allocator alloc;
			alloc.deallocate(this->m_data, this->m_capacity);
		}
	}

	template<typename T, typename Allocator>
	constexpr SPSCQueue<T, Allocator>& SPSCQueue<T, Allocator>::operator=(const SPSCQueue& other)
	{
		other.deepCopy(*this);
		return *this;
	}
	template<typename T, typename Allocator>
	constexpr SPSCQueue<T, Allocator>& SPSCQueue<T, Allocator>::operator=(SPSCQueue&& other) noexcept
	{
		this->m_data = other.m_data;
		this->m_frontOffset = other.m_frontOffset;
		this->m_backOffset = other.m_backOffset;
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;

		other.m_data = nullptr;
		other.m_frontOffset = 0;
		other.m_backOffset = 0;
		other.m_size = 0;
		other.m_capacity = 0;

		return *this;
	}

	template<typename T, typename Allocator>
	constexpr void SPSCQueue<T, Allocator>::enqueue(const T& value)
	{
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		this->m_data[this->m_backOffset] = value;
		//this->m_backOffset = (this->m_backOffset + 1) & this->m_capacity;
		this->m_backOffset += 1;
		this->m_backOffset %= this->m_capacity;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr T SPSCQueue<T, Allocator>::dequeue()
	{
		#ifdef FSDS_DEBUG
			if(this->m_size == 0) [[unlikely]]
			{
				throw std::out_of_range("SPSCQeueue::dequeue called with no available data in the queue");
			}
		#endif
		T* elem = this->m_data + this->m_frontOffset;
		this->m_frontOffset = (this->m_frontOffset + 1) % this->m_capacity;
		this->m_size--;
		return *elem;
	}
	template<typename T, typename Allocator>
	constexpr bool SPSCQueue<T, Allocator>::tryDequeue(T* dest)
	{
		#ifdef FSDS_DEBUG
			if(dest == nullptr) [[unlikely]]
			{
				throw std::invalid_argument("SPSCQeueue::tryDequeue called with nullptr");
			}
		#endif
		if(this->m_size > 0)
		{
			*dest = this->m_data[this->m_frontOffset];
			this->m_frontOffset = (this->m_frontOffset + 1) % this->m_capacity;
			this->m_size--;
			return true;
		}
		else
		{
			return false;
		}
	}
	template<typename T, typename Allocator>
	constexpr T& SPSCQueue<T, Allocator>::front()
	{
		#ifdef FSDS_DEBUG
			if(this->m_size == 0) [[unlikely]]
			{
				throw std::out_of_range("SPSCQeueue::front called with no available data in the queue");
			}
		#endif
		return this->m_data[this->m_frontOffset];
	}

	template<typename T, typename Allocator>
	constexpr T& SPSCQueue<T, Allocator>::operator[](size_t pos)
	{
		#ifdef FSDS_DEBUG
			if(pos >= this->m_size) [[unlikely]]
			{
				throw std::out_of_range("SPSCQeueue::operator[] out of range");
			}
		#endif
		return this->m_data[this->m_frontOffset + pos];
	}
	template<typename T, typename Allocator>
	constexpr const T& SPSCQueue<T, Allocator>::operator[](size_t pos) const
	{
		#ifdef FSDS_DEBUG
			if(pos >= this->m_size) [[unlikely]]
			{
				throw std::out_of_range("SPSCQeueue::operator[] out of range");
			}
		#endif
		return this->m_data[this->m_frontOffset + pos];
	}

	template<typename T, typename Allocator>
	[[nodiscard]] constexpr bool SPSCQueue<T, Allocator>::isEmpty() const noexcept
	{
		return this->m_size == 0;
	}
	template<typename T, typename Allocator>
	constexpr size_t SPSCQueue<T, Allocator>::size() const noexcept
	{
		return this->m_size;
	}
	template<typename T, typename Allocator>
	constexpr size_t SPSCQueue<T, Allocator>::capacity() const noexcept
	{
		return this->m_capacity;
	}

	template<typename T, typename Allocator>
	constexpr void SPSCQueue<T, Allocator>::reserve(const size_t& newCap)
	{
		if(newCap > this->m_size)
		{
			this->reallocate(newCap);
		}
	}
	template<typename T, typename Allocator>
	constexpr void SPSCQueue<T, Allocator>::clear()
	{
		Allocator alloc;
		alloc.deallocate(this->m_data, this->m_capacity);
		this->m_data = alloc.allocate(SPSCQueue<T, Allocator>::sm_baseAllocation);
		this->m_frontOffset = 0;
		this->m_backOffset = 0;
		this->m_size = 0;
		this->m_capacity = SPSCQueue<T, Allocator>::sm_baseAllocation;
	}


	template<typename T, typename Allocator>
	constexpr bool SPSCQueue<T, Allocator>::valueEquality(const SPSCQueue<T, Allocator>& other) const
	{
		//O(n) value equality linear search
		if(this->m_size == other.m_size)
		{
			for(size_t i = 0; i < this->m_size; i++)
			{
				if((*this)[i] != other[i])
				{
					return false;
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename T, typename Allocator>
	constexpr void SPSCQueue<T, Allocator>::deepCopy(SPSCQueue<T, Allocator>& dest) const
	{
		Allocator alloc;
		if(dest.m_data != nullptr)
		{
			alloc.deallocate(dest.m_data, dest.m_capacity);
		}
		dest.m_data = alloc.allocate(this->m_size);
		if(this->m_backOffset > this->m_frontOffset)
		{
			std::copy(this->m_data + this->m_frontOffset, this->m_data + this->m_backOffset, dest.m_data);
		}
		else
		{
			std::copy(this->m_data + this->m_frontOffset, this->m_data + this->m_capacity, dest.m_data);
			std::copy(this->m_data, this->m_data + this->m_backOffset, dest.m_data + (this->m_capacity - this->m_frontOffset));
		}
		dest.m_size = this->m_size;
		dest.m_capacity = this->m_size;
		dest.m_frontOffset = 0;
		dest.m_backOffset = this->m_size;
	}

	template<typename T, typename Allocator>
	constexpr void SPSCQueue<T, Allocator>::reallocate(const size_t& newSize)
	{
		Allocator alloc;
		T* oldData = this->m_data;
		this->m_data = alloc.allocate(newSize);
		//if the data wraps around then it needs to be coppied in two blocks straightening it out
		//e.g. [5, 6, 1, 2, 3, 4] becomes [1, 2, 3, 4, 5, 6]
		if(this->m_backOffset > this->m_frontOffset)
		{
			std::copy(oldData + this->m_frontOffset, oldData + this->m_backOffset, this->m_data);
		}
		else
		{
			std::copy(oldData + this->m_frontOffset, oldData + this->m_capacity, this->m_data);
			std::copy(oldData, oldData + this->m_backOffset, this->m_data + (this->m_capacity - this->m_frontOffset));
		}
		alloc.deallocate(oldData, this->m_capacity);
		this->m_capacity = newSize;
		this->m_frontOffset = 0;
		this->m_backOffset = this->m_size;
	}

	template<typename T, typename Allocator>
	constexpr bool operator==(const SPSCQueue<T, Allocator>& lhs, const SPSCQueue<T, Allocator>& rhs)
	{
		return lhs.valueEquality(rhs);
	}
	template<typename T, typename Allocator>
	constexpr bool operator!=(const SPSCQueue<T, Allocator>& lhs, const SPSCQueue<T, Allocator>& rhs)
	{
		return !(lhs == rhs);
	}
}