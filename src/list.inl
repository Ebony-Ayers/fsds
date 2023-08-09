namespace fsds
{
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List() noexcept(noexcept(Allocator()))
	: m_data(nullptr)//, m_size(0), m_capacity(List<T, Allocator>::sm_baseAllocation)
	{
		this->m_header.size = 0;
		this->m_header.capacity = List<T, Allocator>::sm_baseAllocation;
		Allocator alloc;
		this->m_data = alloc.allocate(List<T, Allocator>::sm_baseAllocation);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(const size_t& count)
	: m_data(nullptr)//, m_size(count), m_capacity(count)
	{
		this->m_header.size = count;
		this->m_header.capacity = count;
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
		this->m_header.size = other.m_header.size;
		this->m_header.capacity = other.m_header.capacity;

		other.m_data = nullptr;
		other.m_header.size = 0;
		other.m_header.capacity = 0;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::List(std::initializer_list<T> init, const Allocator& alloc)
	: m_data(nullptr)//, m_size(init.size()), m_capacity(init.size())
	{
		this->m_header.size = init.size();
		this->m_header.capacity = init.size();
		this->m_data = alloc.allocate(init.size());
		std::copy(init.begin(), init.end(), this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>::~List()
	{
		this->deallocate();
		this->deconstructAll();
	}

	template<typename T, typename Allocator>
	constexpr List<T, Allocator>& List<T, Allocator>::operator=(const List& other)
	{
		this->deallocate();
		this->deconstructAll();

		other.deepCopy(*this);
		return *this;
	}
	template<typename T, typename Allocator>
	constexpr List<T, Allocator>& List<T, Allocator>::operator=(List&& other) noexcept
	{
		this->deallocate();
		this->deconstructAll();

		this->m_data = other.m_data;
		this->m_header.size = other.m_header.size;
		this->m_header.capacity = other.m_header.capacity;

		other.m_data = nullptr;
		other.m_header.size = 0;
		other.m_header.capacity = 0;

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
		return fsds::listInternalFunctions::elementAccessOperator(this->m_header, this->m_data, pos);
	}
	template<typename T, typename Allocator>
	constexpr const T& List<T, Allocator>::operator[](size_t pos) const
	{
		return fsds::listInternalFunctions::constElementAccessOperator(this->m_header, this->m_data, pos);
	}

	template<typename T, typename Allocator>
	constexpr T& List<T, Allocator>::front()
	{
		return fsds::listInternalFunctions::front(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr const T& List<T, Allocator>::front() const
	{
		return fsds::listInternalFunctions::constFront(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr T& List<T, Allocator>::back()
	{
		return fsds::listInternalFunctions::back(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr const T& List<T, Allocator>::back() const
	{
		return fsds::listInternalFunctions::constBack(this->m_header, this->m_data);
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
		return fsds::listInternalFunctions::isEmpty(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr size_t List<T, Allocator>::size() const noexcept
	{
		return fsds::listInternalFunctions::size(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr size_t List<T, Allocator>::maxSize() const noexcept
	{
		return fsds::listInternalFunctions::maxSize(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr size_t List<T, Allocator>::capacity() const noexcept
	{
		return fsds::listInternalFunctions::capacity(this->m_header, this->m_data);
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::reserve(size_t newCap)
	{
		if(newCap > this->m_header.capacity)
		{
			this->reallocate(newCap);
		}
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::clear()
	{
		fsds::listInternalFunctions::clear(this->m_header, this->m_data);
	}
	
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::append(const T& value)
	{
		//test to see if allocation helpers work
		const size_t oldCapacity = this->m_header.capacity;
		T* newData = fsds::listInternalFunctions::reallocationAllocateHelper<std::allocator<T>>(this->m_header, this->m_data, this->getExternalReallocateMinimumRequiredSpace()*2);
		fsds::listInternalFunctions::append(this->m_header, this->m_data, newData, value);
		fsds::listInternalFunctions::reallocationDeallocateHelper<std::allocator<T>>(this->m_header, this->m_data, newData, oldCapacity);
		this->m_data = newData;
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::prepend(const T& value)
	{
		if(this->m_header.size >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::prepend(this->m_header, this->m_data, this->m_data, value);
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::insert(size_t pos, const T& value)
	{
		if(this->m_header.size >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::insert(this->m_header, this->m_data, this->m_data, pos, value);
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void List<T, Allocator>::appendConstruct(Args&&... args)
	{
		if(this->m_header.size >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::appendConstruct(this->m_header, this->m_data, this->m_data, args...);
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void List<T, Allocator>::prependConstruct(Args&&... args)
	{
		if(this->m_header.size >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::prependConstruct(this->m_header, this->m_data, this->m_data, args...);
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void List<T, Allocator>::insertConstruct(size_t pos, Args&&... args)
	{
		if(this->m_header.size >= this->m_header.capacity)
		{
			this->reallocate(this->m_header.capacity * 2);
		}
		fsds::listInternalFunctions::insertConstruct(this->m_header, this->m_data, this->m_data, pos, args...);
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::remove(size_t pos)
	{
		fsds::listInternalFunctions::remove(this->m_header, this->m_data, pos);
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::removeBack()
	{
		fsds::listInternalFunctions::removeBack(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::removeFront()
	{
		fsds::listInternalFunctions::removeFront(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::removeDeconstruct(size_t pos)
	{
		fsds::listInternalFunctions::removeDeconstruct(this->m_header, this->m_data, pos);
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::removeBackDeconstruct()
	{
		fsds::listInternalFunctions::removeBackDeconstruct(this->m_header, this->m_data);
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::removeFrontDeconstruct()
	{
		fsds::listInternalFunctions::removeFrontDeconstruct(this->m_header, this->m_data);
	}

	template<typename T, typename Allocator>
	constexpr bool List<T, Allocator>::valueEquality(const List<T, Allocator>& other) const
	{
		return fsds::listInternalFunctions::valueEquality<T>(this->m_header, this->m_data, other.m_header, other.m_data);
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::deepCopy(List<T, Allocator>& dest) const
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
	constexpr size_t List<T, Allocator>::getExternalReallocateMinimumRequiredSpace()
	{
		return (this->m_header.capacity+1) * sizeof(T);
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::externalReallocate(void* ptr, size_t newSizeBytes)
	{
		size_t newCapacity = newSizeBytes / sizeof(T);
		if(newCapacity >= this->m_header.size) [[likely]]
		{
			std::copy(this->m_data, this->m_data+this->m_header.size, reinterpret_cast<T*>(ptr));
			this->m_header.capacity = newCapacity;
		}
	}

	template<typename T, typename Allocator>
	void List<T, Allocator>::reallocate(size_t newSize)
	{
		Allocator alloc;
		T* oldData = this->m_data;
		this->m_data = alloc.allocate(newSize);
		std::copy(oldData, oldData + this->m_header.size, this->m_data);
		alloc.deallocate(oldData, this->m_header.capacity);
		this->m_header.capacity = newSize;
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::deallocate()
	{
		if(this->m_data != nullptr)
		{
			Allocator alloc;
			alloc.deallocate(this->m_data, this->m_header.capacity);
			this->m_data = nullptr;
			this->m_header.size = 0;
			this->m_header.capacity = 0;
		}
	}

	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::deconstructElement(T* element)
	{
		if(!std::is_trivially_destructible<T>::value)
		{
			std::destroy_at(element);
		}
	}
	template<typename T, typename Allocator>
	constexpr void List<T, Allocator>::deconstructAll()
	{
		if(!std::is_trivially_destructible<T>::value)
		{
			std::destroy(this->m_data, this->m_data + this->m_header.size);
		}
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