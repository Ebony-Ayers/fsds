namespace fsds
{
	template<typename T, typename Allocator>
	constexpr InlineList<T, Allocator>::InlineList() noexcept(noexcept(Allocator()))
	{
		Allocator alloc;
		size_t numBytesToAllocate = InlineListDataBlock<T>::calculateRuntimeSizeOf(InlineList<T, Allocator>::sm_baseAllocation);
		//std::cout << "numBytesToAllocate = " << numBytesToAllocate << std::endl;
		auto res = alloc.allocate_at_least(numBytesToAllocate);
		this->m_dataBlock = reinterpret_cast<fsds::InlineListDataBlock<T>*>(res.ptr);
		//std::cout << "res.count = " << res.count << std::endl;
		size_t allocatedCapacity = InlineListDataBlock<T>::calculateNumElementsFromSize(res.count);
		//std::cout << "allocatedCapacity = " << allocatedCapacity << std::endl;
		*(this->m_dataBlock->getHeader()) = {0, allocatedCapacity, 0};
	}
	template<typename T, typename Allocator>
	constexpr InlineList<T, Allocator>::InlineList(const size_t& count)
	{
		Allocator alloc;
		size_t numBytesToAllocate = InlineListDataBlock<T>::calculateRuntimeSizeOf(count);
		auto res = alloc.allocate_at_least(numBytesToAllocate);
		this->m_dataBlock = reinterpret_cast<fsds::InlineListDataBlock<T>*>(res.ptr);
		size_t allocatedCapacity = InlineListDataBlock<T>::calculateNumElementsFromSize(res.count);
		*(this->m_dataBlock->getHeader()) = {0, allocatedCapacity, 0};
	}
	template<typename T, typename Allocator>
	constexpr InlineList<T, Allocator>::InlineList(const InlineList& other)
	{
		//
	}
	template<typename T, typename Allocator>
	constexpr InlineList<T, Allocator>::InlineList(InlineList&& other) noexcept
	{
		//
	}
	template<typename T, typename Allocator>
	constexpr InlineList<T, Allocator>::InlineList(std::initializer_list<T> init, const Allocator& alloc)
	{
		//
	}
	template<typename T, typename Allocator>
	constexpr InlineList<T, Allocator>::~InlineList()
	{
		if(this->m_dataBlock != nullptr) [[likely]]
		{
			this->deconstructAll();
			Allocator alloc;
			size_t numBytesToDeallocate = InlineListDataBlock<T>::calculateRuntimeSizeOf(this->m_dataBlock->getHeader()->capacity);
			alloc.deallocate(this->m_dataBlock, numBytesToDeallocate);
		}
	}


	template<typename T, typename Allocator>
	constexpr T& InlineList<T, Allocator>::operator[](size_t pos)
	{
		return this->m_dataBlock->get(pos);
	}
	template<typename T, typename Allocator>
	constexpr const T& InlineList<T, Allocator>::operator[](size_t pos) const
	{
		return this->m_dataBlock->get(pos);
	}

	template<typename T, typename Allocator>
	constexpr Allocator InlineList<T, Allocator>::getAllocator() const noexcept
	{
		Allocator alloc;
		return alloc;
	}

	template<typename T, typename Allocator>
	constexpr T& InlineList<T, Allocator>::front()
	{
		return this->m_dataBlock->front();
	}
	template<typename T, typename Allocator>
	constexpr const T& InlineList<T, Allocator>::front() const
	{
		return this->m_dataBlock->front();
	}
	template<typename T, typename Allocator>
	constexpr T& InlineList<T, Allocator>::back()
	{
		return this->m_dataBlock->back();
	}
	template<typename T, typename Allocator>
	constexpr const T& InlineList<T, Allocator>::back() const
	{
		return this->m_dataBlock->back();
	}

	template<typename T, typename Allocator>
	constexpr T* InlineList<T, Allocator>::data()
	{
		return this->m_dataBlock->getData();
	}
	template<typename T, typename Allocator>
	constexpr const T* InlineList<T, Allocator>::data() const
	{
		return this->m_dataBlock->getData();
	}
	template<typename T, typename Allocator>
	constexpr InlineListDataBlock<T>* InlineList<T, Allocator>::getInlineDataBlock()
	{
		return this->m_dataBlock;
	}
	template<typename T, typename Allocator>
	constexpr const InlineListDataBlock<T>* InlineList<T, Allocator>::getInlineDataBlock() const
	{
		return this->m_dataBlock;
	}


	template<typename T, typename Allocator>
	[[nodiscard]] constexpr bool InlineList<T, Allocator>::isEmpty() const noexcept
	{
		return this->m_dataBlock->isEmpty();
	}
	template<typename T, typename Allocator>
	constexpr size_t InlineList<T, Allocator>::size() const noexcept
	{
		return this->m_dataBlock->size();
	}
	template<typename T, typename Allocator>
	constexpr size_t InlineList<T, Allocator>::maxSize() const noexcept
	{
		return this->m_dataBlock->maxSize();
	}
	template<typename T, typename Allocator>
	constexpr size_t InlineList<T, Allocator>::capacity() const noexcept
	{
		return this->m_dataBlock->capacity();
	}


	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::reserve(size_t newCap)
	{
		auto newList = this->allocateElements(newCap);
		std::copy(this->m_dataBlock->getUnoffsetedDataPtr() + this->m_dataBlock->getHeader()->front, this->m_dataBlock->getUnoffsetedDataPtr() + this->m_dataBlock->getHeader()->front + this->m_dataBlock->getHeader()->size, newList->getUnoffsetedDataPtr());
		newList->getHeader()->size = this->m_dataBlock->getHeader()->size;
		newList->getHeader()->front = 0;
		this->deallocate(this->m_dataBlock);
		this->m_dataBlock = newList;
	}
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::clear()
	{
		this->deconstructAll();
		this->m_dataBlock->getHeader()->size = 0;
		this->m_dataBlock->getHeader()->front = 0;
	}


	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::append(const T& value)
	{
		if(this->getEffectiveSize() >= this->m_dataBlock->capacity())
		{
			auto newList = this->reallocate();
			this->m_dataBlock->copyAppend(value, newList);
			this->deallocate(this->m_dataBlock);
			this->m_dataBlock = newList;
		}
		else
		{
			this->m_dataBlock->safeAppend(value);
		}
	}
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::prepend(const T& value)
	{
		if(this->getEffectiveSize() >= this->m_dataBlock->capacity())
		{
			auto newList = this->reallocate();
			this->m_dataBlock->copyPrepend(value, newList);
			this->deallocate(this->m_dataBlock);
			this->m_dataBlock = newList;
		}
		else
		{
			this->m_dataBlock->safePrepend(value);
		}
	}
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::insert(size_t pos, const T& value)
	{
		if(this->getEffectiveSize() >= this->m_dataBlock->capacity())
		{
			auto newList = this->reallocate();
			this->m_dataBlock->copyInsert(pos, value, newList);
			this->deallocate(this->m_dataBlock);
			this->m_dataBlock = newList;
		}
		else
		{
			this->m_dataBlock->safeInsert(pos, value);
		}
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void InlineList<T, Allocator>::appendConstruct(Args&&... args)
	{
		if(this->getEffectiveSize() >= this->m_dataBlock->capacity())
		{
			auto newList = this->reallocate();
			this->m_dataBlock->copyAppendConstruct(args..., newList);
			this->deallocate(this->m_dataBlock);
			this->m_dataBlock = newList;
		}
		else
		{
			this->m_dataBlock->safeAppendConstruct(args...);
		}
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void InlineList<T, Allocator>::prependConstruct(Args&&... args)
	{
		if(this->getEffectiveSize() >= this->m_dataBlock->capacity())
		{
			auto newList = this->reallocate();
			this->m_dataBlock->copyPrependConstruct(args..., newList);
			this->deallocate(this->m_dataBlock);
			this->m_dataBlock = newList;
		}
		else
		{
			this->m_dataBlock->safePrependConstruct(args...);
		}
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void InlineList<T, Allocator>::insertConstruct(size_t pos, Args&&... args)
	{
		if(this->getEffectiveSize() >= this->m_dataBlock->capacity())
		{
			auto newList = this->reallocate();
			this->m_dataBlock->copyInsert(pos, args..., newList);
			this->deallocate(this->m_dataBlock);
			this->m_dataBlock = newList;
		}
		else
		{
			this->m_dataBlock->copyInsert(pos, args...);
		}
	}


	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::remove(size_t pos)
	{
		this->m_dataBlock->remove(pos);
	}
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::removeBack()
	{
		this->m_dataBlock->removeBack();
	}
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::removeFront()
	{
		this->m_dataBlock->removeFront();
	}
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::removeWithoutDeconstruct(size_t pos)
	{
		this->m_dataBlock->removeWithoutDeconstruct(pos);
	}
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::removeBackWithoutDeconstruct()
	{
		this->m_dataBlock->removeWithoutDeconstruct();
	}
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::removeFrontWithoutDeconstruct()
	{
		this->m_dataBlock->removeFrontWithoutDeconstruct();
	}

	template<typename T, typename Allocator>
	constexpr bool InlineList<T, Allocator>::valueEquality(const InlineList<T, Allocator>& other) const
	{
		return fsds::listInternalFunctions::valueEquality(*this->m_dataBlock->getHeader(), this->m_dataBlock->getUnoffsetedDataPtr(), *other.getInlineDataBlock()->getHeader(), other.m_dataBlock->getUnoffsetedDataPtr());
	}

	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::deepCopy(InlineList<T, Allocator>& dest) const
	{
		dest.clear();
		dest.reserve(this->m_dataBlock->getHeader()->size);
		fsds::listInternalFunctions::deepCopy(*this->m_dataBlock->getHeader(), this->m_dataBlock->getUnoffsetedDataPtr(), *dest.getInlineDataBlock()->getHeader(), dest.m_dataBlock->getUnoffsetedDataPtr());
	}


	//simple wrapper around memory allocation
	template<typename T, typename Allocator>
	constexpr InlineList<T, Allocator>::allocationByteResult InlineList<T, Allocator>::allocateBytes(size_t size)
	{
		Allocator alloc;
		allocationByteResult output;
		auto res = alloc.allocate_at_least(size);
		output.newList = reinterpret_cast<fsds::InlineListDataBlock<T>*>(res.ptr);
		output.numBytesAllocated = res.count;
		return output;
	}
	//allocate at least enough memory for a given number of elements and return that list
	template<typename T, typename Allocator>
	constexpr InlineListDataBlock<T>* InlineList<T, Allocator>::allocateElements(size_t size)
	{
		//compute how many bytes we should allocate for the allocation
		size_t numBytesToAllocate = InlineListDataBlock<T>::calculateRuntimeSizeOf(size);
		auto res = this->allocateBytes(numBytesToAllocate);
		//compute the number of elements actually allocated as it may be larger than the amount we requested
		size_t allocatedCapacity = InlineListDataBlock<T>::calculateNumElementsFromSize(res.numBytesAllocated);
		//initialise the header
		*(res.newList) = {0, allocatedCapacity, 0};
		return res.newList;
	}
	//allocate a new list larger than the current and return it
	template<typename T, typename Allocator>
	constexpr InlineListDataBlock<T>* InlineList<T, Allocator>::reallocate()
	{
		Allocator alloc;
		//compute how many bytes we should allocate for the reallocation
		size_t numBytesToAllocate = this->m_dataBlock->runtimeSizeOfDouble();
		auto res = this->allocateBytes(numBytesToAllocate);
		//compute the number of elements actually allocated as it may be larger than the amount we requested
		size_t allocatedCapacity = InlineListDataBlock<T>::calculateNumElementsFromSize(res.numBytesAllocated);
		//initialise the header
		*(res.newList) = {0, allocatedCapacity, 0};
		return res.newList;
	}
	//simple wrapper around memory deallocation which matches the allocation wrapper
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::deallocate(InlineListDataBlock<T>* oldList)
	{
		Allocator alloc;
		alloc.deallocate(oldList, oldList->getHeader()->capacity);
	}
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::deconstructElement(T* element)
	{
		if(!std::is_trivially_destructible<T>::value)
		{
			std::destroy_at(element);
		}
	}
	template<typename T, typename Allocator>
	constexpr void InlineList<T, Allocator>::deconstructAll()
	{
		if(!std::is_trivially_destructible<T>::value)
		{
			std::destroy(this->m_dataBlock->getUnoffsetedDataPtr() + this->m_dataBlock->getHeader()->front, this->m_dataBlock->getUnoffsetedDataPtr() + this->m_dataBlock->getHeader()->front + this->m_dataBlock->getHeader()->size);
		}
	}

	template<typename T, typename Allocator>
	constexpr size_t InlineList<T, Allocator>::getEffectiveSize() const
	{
		return this->m_dataBlock->getHeader()->size + this->m_dataBlock->getHeader()->front;
	}
}