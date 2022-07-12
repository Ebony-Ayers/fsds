namespace fsds
{
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List() noexcept(noexcept(Allocator()))
	: m_data(nullptr), m_size(0), m_capacity(List<T, Allocator>::sm_baseAllocation)
	{
		Allocator alloc;
		this->m_data = alloc.allocate(List<T, Allocator>::sm_baseAllocation);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(const size_t& count)
	: m_data(nullptr), m_size(count), m_capacity(count)
	{
		Allocator alloc;
		this->m_data = alloc.allocate(count);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(const List& other)
	{
		this->m_data = nullptr;
		other.deepCopy(*this);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(List&& other) noexcept
	{
		this->m_data = other.m_data;
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;

		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(std::initializer_list<T> init, const Allocator& alloc)
	: m_data(nullptr), m_size(init.size()), m_capacity(init.size())
	{
		this->m_data = alloc.allocate(init.size());
		std::copy(init.begin(), init.end(), this->m_data);
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
		other.deepCopy(*this);
		return *this;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>& List<T, Allocator>::operator=(List&& other) noexcept
	{
		this->m_data = other.m_data;
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;

		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;

		return *this;
	}

	template<typename T, typename Allocator>
	constexpr Allocator List<T, Allocator>::getAllocator() const noexcept
	{
		Allocator alloc;
		return alloc;
	}

	template<typename T, typename Allocator>
	constexpr T& List<T, Allocator>::operator[](size_t pos)
	{
		return this->m_data[pos];
	}
	template<typename T, typename Allocator>
	constexpr const T& List<T, Allocator>::operator[](size_t pos) const
	{
		return this->m_data[pos];
	}

	template<typename T, typename Allocator>
	constexpr T& List<T, Allocator>::front()
	{
		return this->m_data[0];
	}
	template<typename T, typename Allocator>
	constexpr const T& List<T, Allocator>::front() const
	{
		return this->m_data[0];
	}
	template<typename T, typename Allocator>
	constexpr T& List<T, Allocator>::back()
	{
		return this->m_data[this->m_size - 1];
	}
	template<typename T, typename Allocator>
	constexpr const T& List<T, Allocator>::back() const
	{
		return this->m_data[this->m_size - 1];
	}

	template<typename T, typename Allocator>
	constexpr T* List<T, Allocator>::data()
	{
		return this->m_data;
	}
	template<typename T, typename Allocator>
	constexpr const T* List<T, Allocator>::data() const
	{
		return this->m_data;
	}

	template<typename T, typename Allocator>
	[[nodiscard]] constexpr bool List<T, Allocator>::isEmpty() const noexcept
	{
		return this->m_size == 0;
	}
	template<typename T, typename Allocator>
	constexpr size_t List<T, Allocator>::size() const noexcept
	{
		return this->m_size;
	}
	template<typename T, typename Allocator>
	constexpr size_t List<T, Allocator>::maxSize() const noexcept
	{
		return std::numeric_limits<size_t>::max() / sizeof(T);
	}
	template<typename T, typename Allocator>
	constexpr size_t List<T, Allocator>::capacity() const noexcept
	{
		return this->m_capacity;
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::reserve(size_t newCap)
	{
		if(newCap > this->m_capacity)
		{
			this->reallocate(newCap);
		}
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::clear()
	{
		Allocator alloc;
		alloc.deallocate(this->m_data, this->m_capacity);
		this->m_data = alloc.allocate(List<T, Allocator>::sm_baseAllocation);
		this->m_size = 0;
		this->m_capacity = List<T, Allocator>::sm_baseAllocation;
	}
	
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::append(const T& value)
	{
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		this->m_data[this->m_size] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void List<T, Allocator>::appendConstruct(Args&&... args)
	{
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		this->m_data[this->m_size] = T(args...);
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::prepend(const T& value)
	{
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
	constexpr void List<T, Allocator>::prependConstruct(Args&&... args)
	{
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		std::copy(this->m_data, this->m_data + this->m_size, this->m_data + 1);
		this->m_data[0] = T(args...);
		this->m_size++;
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::insert(size_t pos, const T& value)
	{
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		this->m_data[pos] = value;
		this->m_size++;
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void List<T, Allocator>::insertConstruct(size_t pos, Args&&... args)
	{
		if(this->m_size >= this->m_capacity)
		{
			this->reallocate(this->m_capacity * 2);
		}
		std::copy(this->m_data + pos, this->m_data + this->m_size, this->m_data + pos + 1);
		this->m_data[pos] = T(args...);
		this->m_size++;
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::remove(size_t pos)
	{
		this->m_data[pos].~T();
		std::copy(this->m_data + pos + 1, this->m_data + this->m_size, this->m_data + pos);
		this->m_size--;
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::removeBack()
	{
		this->m_data[this->m_size - 1].~T();
		this->m_size--;
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::removeFront()
	{
		this->m_data[0].~T();
		std::copy(this->m_data + 1, this->m_data + this->m_size, this->m_data);
		this->m_size--;
	}

	template<typename T, typename Allocator>
	constexpr bool List<T, Allocator>::valueEquality(const List<T, Allocator>& other) const
	{
		//O(n) value equality linear search
		if(this->m_size == other.m_size)
		{
			for(size_t i = 0; i < this->m_size; i++)
			{
				if(this->m_data[i] != other.m_data[i])
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
	constexpr void List<T, Allocator>::deepCopy(List<T, Allocator>& dest) const
	{
		if(dest.m_data != nullptr)
		{
			dest.getAllocator().deallocate(dest.m_data, dest.m_capacity);
		}
		Allocator alloc;
		dest.m_data = alloc.allocate(this->m_size);
		std::copy(this->m_data, this->m_data + this->m_size, dest.m_data);
		dest.m_size = this->m_size;
		dest.m_capacity = this->m_capacity;
	}

	template<typename T, typename Allocator>
	void List<T, Allocator>::reallocate(size_t newSize)
	{
		Allocator alloc;
		T* oldData = this->m_data;
		this->m_data = alloc.allocate(newSize);
		std::copy(oldData, oldData + this->m_size, this->m_data);
		alloc.deallocate(oldData, this->m_capacity);
		this->m_capacity = newSize;
	}

	template<typename T, typename Allocator>
	constexpr bool operator==(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs)
	{
		return lhs.valueEquality(rhs);
	}
	template<typename T, typename Allocator>
	constexpr bool operator!=(const List<T, Allocator>& lhs, const List<T, Allocator>& rhs)
	{
		return !(lhs == rhs);
	}
}