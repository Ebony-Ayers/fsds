namespace fsds
{
	template<typename T, typename Allocator>
	constexpr SeperateDataList<T, Allocator>::SeperateDataList() noexcept(noexcept(Allocator()))
	: m_data(nullptr), m_header({0, SeperateDataList<T, Allocator>::sm_baseAllocation, 0})
	{
		Allocator alloc;
		this->m_data = alloc.allocate(SeperateDataList<T, Allocator>::sm_baseAllocation);
	}
	template<typename T, typename Allocator>
	constexpr SeperateDataList<T, Allocator>::SeperateDataList(const size_t& count)
	: m_data(nullptr), m_header({0, count, 0}) //bug where initial size was set to the initial capacity not zero was not caught by testing
	{
		Allocator alloc;
		this->m_data = alloc.allocate(count);
	}
	template<typename T, typename Allocator>
	constexpr SeperateDataList<T, Allocator>::SeperateDataList(const SeperateDataList& other)
	: m_data(nullptr), m_header({0, 0, 0})
	{
		other.deepCopy(*this);
	}
	template<typename T, typename Allocator>
	constexpr SeperateDataList<T, Allocator>::SeperateDataList(SeperateDataList&& other) noexcept
	: m_data(other.m_data), m_header({other.m_header.size, other.m_header.capacity, other.m_header.front})
	{
		other.m_data = nullptr;
		other.m_header.size = 0;
		other.m_header.capacity = 0;
		other.m_header.front = 0;
	}
	template<typename T, typename Allocator>
	constexpr SeperateDataList<T, Allocator>::SeperateDataList(std::initializer_list<T> init, const Allocator& alloc)
	: m_data(nullptr), m_header({init.size(), init.size(), 0})
	{
		this->m_data = alloc.allocate(init.size());
		std::copy(init.begin(), init.end(), this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr SeperateDataList<T, Allocator>::~SeperateDataList()
	{
		this->deconstructAll();
		this->deallocate();
	}

	template<typename T, typename Allocator>
	constexpr SeperateDataList<T, Allocator>& SeperateDataList<T, Allocator>::operator=(const SeperateDataList& other)
	{
		this->deallocate();
		this->deconstructAll();

		other.deepCopy(*this);
		return *this;
	}
	template<typename T, typename Allocator>
	constexpr SeperateDataList<T, Allocator>& SeperateDataList<T, Allocator>::operator=(SeperateDataList&& other) noexcept
	{
		this->deallocate();
		this->deconstructAll();

		this->m_data = other.m_data;
		this->m_header.size = other.m_header.size;
		this->m_header.capacity = other.m_header.capacity;
		this->m_header.front = other.m_header.front;

		other.m_data = nullptr;
		other.m_header.size = 0;
		other.m_header.capacity = 0;
		other.m_header.front = 0;

		return *this;
	}

	template<typename T, typename Allocator>
	constexpr Allocator SeperateDataList<T, Allocator>::getAllocator() const noexcept
	{
		Allocator alloc;
		return alloc;
	}

	template<typename T, typename Allocator>
	constexpr T& SeperateDataList<T, Allocator>::operator[](size_t pos)
	{
		return fsds::listInternalFunctions::elementAccessOperator(this->m_header, this->m_data, pos);
	}
	template<typename T, typename Allocator>
	constexpr const T& SeperateDataList<T, Allocator>::operator[](size_t pos) const
	{
		return fsds::listInternalFunctions::constElementAccessOperator(this->m_header, this->m_data, pos);
	}

	template<typename T, typename Allocator>
	constexpr T& SeperateDataList<T, Allocator>::front()
	{
		return fsds::listInternalFunctions::front(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr const T& SeperateDataList<T, Allocator>::front() const
	{
		return fsds::listInternalFunctions::constFront(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr T& SeperateDataList<T, Allocator>::back()
	{
		return fsds::listInternalFunctions::back(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr const T& SeperateDataList<T, Allocator>::back() const
	{
		return fsds::listInternalFunctions::constBack(this->m_header, this->m_data);
	}

	template<typename T, typename Allocator>
	constexpr T* SeperateDataList<T, Allocator>::data()
	{
		return this->m_data;
	}
	template<typename T, typename Allocator>
	constexpr const T* SeperateDataList<T, Allocator>::data() const
	{
		return this->m_data;
	}

	template<typename T, typename Allocator>
	[[nodiscard]] constexpr bool SeperateDataList<T, Allocator>::isEmpty() const noexcept
	{
		return fsds::listInternalFunctions::isEmpty(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr size_t SeperateDataList<T, Allocator>::size() const noexcept
	{
		return fsds::listInternalFunctions::size(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr size_t SeperateDataList<T, Allocator>::maxSize() const noexcept
	{
		return fsds::listInternalFunctions::maxSize(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr size_t SeperateDataList<T, Allocator>::capacity() const noexcept
	{
		return fsds::listInternalFunctions::capacity(this->m_header, this->m_data);
	}

	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::reserve(size_t newCap)
	{
		if(newCap > this->m_header.capacity)
		{
			this->reallocate(newCap);
		}
	}
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::clear()
	{
		fsds::listInternalFunctions::clear(this->m_header, this->m_data);
	}
	
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::append(const T& value)
	{
		if(this->m_header.size + this->m_header.front >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::appendConstruct(this->m_header, this->m_header, this->m_data, this->m_data, value);
	}
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::prepend(const T& value)
	{
		if(this->m_header.size + this->m_header.front >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::prepend(this->m_header, this->m_header, this->m_data, this->m_data, value);
	}
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::insert(size_t pos, const T& value)
	{
		if(this->m_header.size + this->m_header.front >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::insert(this->m_header, this->m_header, this->m_data, this->m_data, pos, value);
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void SeperateDataList<T, Allocator>::appendConstruct(Args&&... args)
	{
		if(this->m_header.size + this->m_header.front >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::appendConstruct(this->m_header, this->m_header, this->m_data, this->m_data, args...);
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void SeperateDataList<T, Allocator>::prependConstruct(Args&&... args)
	{
		if(this->m_header.size + this->m_header.front >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::prependConstruct(this->m_header, this->m_header, this->m_data, this->m_data, args...);
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void SeperateDataList<T, Allocator>::insertConstruct(size_t pos, Args&&... args)
	{
		if(this->m_header.size + this->m_header.front >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::insertConstruct(this->m_header, this->m_header, this->m_data, this->m_data, pos, args...);
	}

	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::remove(size_t pos)
	{
		fsds::listInternalFunctions::removeDeconstruct(this->m_header, this->m_data, pos);
	}
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::removeBack()
	{
		fsds::listInternalFunctions::removeBackDeconstruct(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::removeFront()
	{
		fsds::listInternalFunctions::removeFrontDeconstruct(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::removeDeconstruct(size_t pos)
	{
		fsds::listInternalFunctions::removeWithoutDeconstruct(this->m_header, this->m_data, pos);
	}
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::removeBackDeconstruct()
	{
		fsds::listInternalFunctions::removeBackWithoutDeconstruct(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::removeFrontDeconstruct()
	{
		fsds::listInternalFunctions::removeFrontWithoutDeconstruct(this->m_header, this->m_data);
	}

	template<typename T, typename Allocator>
	constexpr bool SeperateDataList<T, Allocator>::valueEquality(const SeperateDataList<T, Allocator>& other) const
	{
		return fsds::listInternalFunctions::valueEquality<T>(this->m_header, this->m_data, other.m_header, other.m_data);
	}

	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::deepCopy(SeperateDataList<T, Allocator>& dest) const
	{
		if(dest.m_data != nullptr)
		{
			dest.getAllocator().deallocate(dest.m_data, dest.m_header.capacity);
		}
		Allocator alloc;
		dest.m_data = alloc.allocate(this->m_header.size);
		fsds::listInternalFunctions::deepCopy(this->m_header, this->m_data, dest.m_header, dest.m_data);
	}

	template<typename T, typename Allocator>
	constexpr size_t SeperateDataList<T, Allocator>::getExternalReallocateMinimumRequiredSpace()
	{
		return (this->m_header.capacity+1) * sizeof(T);
	}
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::externalReallocate(void* ptr, size_t newSizeBytes)
	{
		size_t newCapacity = newSizeBytes / sizeof(T);
		if(newCapacity >= this->m_header.size) [[likely]]
		{
			std::copy(this->m_data+this->m_header.front, this->m_data+this->m_header.front+this->m_header.size, reinterpret_cast<T*>(ptr));
			this->m_header.capacity = newCapacity;
			this->m_header.front = 0;
		}
	}

	template<typename T, typename Allocator>
	void SeperateDataList<T, Allocator>::reallocate(size_t newSize)
	{
		Allocator alloc;
		T* oldData = this->m_data;
		this->m_data = alloc.allocate(newSize);
		std::copy(oldData + this->m_header.front, oldData + this->m_header.front + this->m_header.size, this->m_data);
		alloc.deallocate(oldData, this->m_header.capacity);
		this->m_header.capacity = newSize;
		this->m_header.front = 0;
	}

	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::deallocate()
	{
		if(this->m_data != nullptr)
		{
			Allocator alloc;
			alloc.deallocate(this->m_data, this->m_header.capacity);
			this->m_data = nullptr;
			this->m_header.size = 0;
			this->m_header.capacity = 0;
			this->m_header.front = 0;
		}
	}

	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::deconstructElement(T* element)
	{
		if(!std::is_trivially_destructible<T>::value)
		{
			std::destroy_at(element);
		}
	}
	template<typename T, typename Allocator>
	constexpr void SeperateDataList<T, Allocator>::deconstructAll()
	{
		if(!std::is_trivially_destructible<T>::value)
		{
			std::destroy(this->m_data, this->m_data + this->m_header.size);
		}
	}

	template<typename T, typename Allocator>
	constexpr bool operator==(const SeperateDataList<T, Allocator>& lhs, const SeperateDataList<T, Allocator>& rhs)
	{
		return lhs.valueEquality(rhs);
	}
	template<typename T, typename Allocator>
	constexpr bool operator!=(const SeperateDataList<T, Allocator>& lhs, const SeperateDataList<T, Allocator>& rhs)
	{
		return !(lhs == rhs);
	}
}