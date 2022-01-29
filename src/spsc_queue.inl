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
	constexpr SPSCQueue<T, Allocator>::~SPSCQueue()
	{
		if(this->m_data != nullptr)
		{
			Allocator alloc;
			alloc.deallocate(this->m_data, this->m_capacity);
		}
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
		T* elem = this->m_data + this->m_frontOffset;
		this->m_frontOffset = (this->m_frontOffset + 1) % this->m_capacity;
		this->m_size--;
		return *elem;
	}
	template<typename T, typename Allocator>
	constexpr T& SPSCQueue<T, Allocator>::front()
	{
		return this->m_data[this->m_frontOffset];
	}

	template<typename T, typename Allocator>
	constexpr T* SPSCQueue<T, Allocator>::operator[](size_t pos)
	{
		return this->m_data + this->m_frontOffset + pos;
	}
	template<typename T, typename Allocator>
	constexpr const T* SPSCQueue<T, Allocator>::operator[](size_t pos) const
	{
		return this->m_data + this->m_frontOffset + pos;
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
	constexpr void SPSCQueue<T, Allocator>::reserve(size_t newCap)
	{
		if(newCap > this->m_capacity)
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
		this->m_size = SPSCQueue<T, Allocator>::sm_baseAllocation;
		this->m_capacity = SPSCQueue<T, Allocator>::sm_baseAllocation;
	}


	template<typename T, typename Allocator>
	constexpr bool SPSCQueue<T, Allocator>::dataReferenceEquality(const SPSCQueue<T, Allocator>& other)
	{
		return (this->m_data == other.m_data) && (this->m_frontOffset == other.m_frontOffset) && (this->m_backOffset == other.m_backOffset) && (this->m_size == other.m_size);
	}
	template<typename T, typename Allocator>
	constexpr bool SPSCQueue<T, Allocator>::valueEquality(const SPSCQueue<T, Allocator>& other)
	{
		//reference equality is a super set of value equality and much cheaper to calculate so dheck first
		if(this->dataReferenceEquality(other))
		{
			return true;
		}
		else
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
	}

	template<typename T, typename Allocator>
	constexpr void SPSCQueue<T, Allocator>::reallocate(size_t newSize)
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

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(const SPSCQueue<T, Allocator>& lhs, const SPSCQueue<T, Allocator>& rhs)
	{
		return lhs.dataReferenceEquality(rhs);
	}
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator!=(const SPSCQueue<T, Allocator>& lhs, const SPSCQueue<T, Allocator>& rhs)
	{
		return !(lhs == rhs);
	}
}