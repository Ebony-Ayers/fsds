namespace fsds
{
	template<typename T>
	constexpr size_t InlineListDataBlock<T>::runtimeSizeOf()
	{
		return sizeof(InlineListDataBlock) + (sizeof(T) * (this->m_header.capacity - 1));
	}
	template<typename T>
	constexpr size_t InlineListDataBlock<T>::runtimeSizeOfDouble()
	{
		return sizeof(InlineListDataBlock) + (sizeof(T) * ((2 * this->m_header.capacity) - 1));
	}
	template<typename T>
	template<double factor>
	constexpr size_t InlineListDataBlock<T>::runtimeSizeOfFactorIncrease()
	{
		return sizeof(InlineListDataBlock) + (sizeof(T) * (static_cast<size_t>(factor * static_cast<double>(this->m_header.capacity)) - 1));
	}


	template<typename T>
	constexpr fsds::listInternalFunctions::ListHeader* InlineListDataBlock<T>::getHeader()
	{
		return &this->m_header;
	}
	template<typename T>
	constexpr const fsds::listInternalFunctions::ListHeader* InlineListDataBlock<T>::getHeader() const
	{
		return &this->m_header;
	}
	template<typename T>
	constexpr T* InlineListDataBlock<T>::getData()
	{
		return this->m_data + this->m_header.front;
	}
	template<typename T>
	constexpr const T* InlineListDataBlock<T>::getData() const
	{
		return this->m_data + this->m_header.front;
	}

	template<typename T>
	constexpr T& InlineListDataBlock<T>::get(size_t pos)
	{
		return fsds::listInternalFunctions::elementAccessOperator(this->m_header, this->m_data, pos);
	}
	template<typename T>
	constexpr const T& InlineListDataBlock<T>::get(size_t pos) const
	{
		return fsds::listInternalFunctions::constElementAccessOperator(this->m_header, this->m_data, pos);
	}

	template<typename T>
	constexpr T& InlineListDataBlock<T>::front()
	{
		return fsds::listInternalFunctions::front(this->m_header, this->m_data);
	}
	template<typename T>
	constexpr const T& InlineListDataBlock<T>::front() const
	{
		return fsds::listInternalFunctions::constFront(this->m_header, this->m_data);
	}
	template<typename T>
	constexpr T& InlineListDataBlock<T>::back()
	{
		return fsds::listInternalFunctions::back(this->m_header, this->m_data);
	}
	template<typename T>
	constexpr const T& InlineListDataBlock<T>::back() const
	{
		return fsds::listInternalFunctions::constBack(this->m_header, this->m_data);
	}

	template<typename T>
	[[nodiscard]] constexpr bool InlineListDataBlock<T>::isEmpty() const noexcept
	{
		return fsds::listInternalFunctions::isEmpty(this->m_header, this->m_data);
	}
	template<typename T>
	constexpr size_t InlineListDataBlock<T>::size() const noexcept
	{
		return fsds::listInternalFunctions::size(this->m_header, this->m_data);
	}
	template<typename T>
	constexpr size_t InlineListDataBlock<T>::maxSize() const noexcept
	{
		return fsds::listInternalFunctions::maxSize(this->m_header, this->m_data);
	}
	template<typename T>
	constexpr size_t InlineListDataBlock<T>::capacity() const noexcept
	{
		return fsds::listInternalFunctions::capacity(this->m_header, this->m_data);
	}

	template<typename T>
	constexpr void InlineListDataBlock<T>::safeAppend(const T& value)
	{
		fsds::listInternalFunctions::append(this->m_header, this->m_data, this->m_data, value);
	}
	template<typename T>
	constexpr void InlineListDataBlock<T>::copyAppend(const T& value, InlineListDataBlock<T>* newList)
	{
		fsds::listInternalFunctions::append(this->m_header, this->m_data, newList->getData());
		newList->getHeader = this->m_header;
	}
	template<typename T>
	constexpr void InlineListDataBlock<T>::safePrepend(const T& value)
	{
		fsds::listInternalFunctions::prepend(this->m_header, this->m_data, this->m_data, value);
	}
	template<typename T>
	constexpr void InlineListDataBlock<T>::copyPrepend(const T& value, InlineListDataBlock<T>* newList)
	{
		fsds::listInternalFunctions::prepend(this->m_header, this->m_data, newList->getData(), value);
		newList->getHeader = this->m_header;
	}
	template<typename T>
	constexpr void InlineListDataBlock<T>::safeInsert(size_t pos, const T& value)
	{
		fsds::listInternalFunctions::insert(this->m_header, this->m_data, this->m_data, pos, value);
	}
	template<typename T>
	constexpr void InlineListDataBlock<T>::copyInsert(size_t pos, const T& value, InlineListDataBlock<T>* newList)
	{
		fsds::listInternalFunctions::insert(this->m_header, this->m_data, newList->getData(), pos, value);
	}

	template<typename T>
	template<typename... Args>
	constexpr void InlineListDataBlock<T>::safeAppendConstruct(Args&&... args)
	{
		fsds::listInternalFunctions::appendConstruct(this->m_header, this->m_data, this->m_data, args...);
	}
	template<typename T>
	template<typename... Args>
	constexpr void InlineListDataBlock<T>::copyAppendConstruct(Args&&... args, InlineListDataBlock<T>* newList)
	{
		fsds::listInternalFunctions::appendConstruct(this->m_header, this->m_data, newList->getData(), args...);
	}
	template<typename T>
	template<typename... Args>
	constexpr void InlineListDataBlock<T>::safePrependConstruct(Args&&... args)
	{
		fsds::listInternalFunctions::prependConstruct(this->m_header, this->m_data, this->m_data, args...);
	}
	template<typename T>
	template<typename... Args>
	constexpr void InlineListDataBlock<T>::copyPrependConstruct(Args&&... args, InlineListDataBlock<T>* newList)
	{
		fsds::listInternalFunctions::prependConstruct(this->m_header, this->m_data, newList->getData(), args...);
	}
	template<typename T>
	template<typename... Args>
	constexpr void InlineListDataBlock<T>::safeInsertConstruct(size_t pos, Args&&... args)
	{
		fsds::listInternalFunctions::insertConstruct(this->m_header, this->m_data, this->m_data, pos, args...);
	}
	template<typename T>
	template<typename... Args>
	constexpr void InlineListDataBlock<T>::copyInsertConstruct(size_t pos, Args&&... args, InlineListDataBlock<T>* newList)
	{
		fsds::listInternalFunctions::insertConstruct(this->m_header, this->m_data, newList->getData(), pos, args...);
	}

	template<typename T>
	constexpr void InlineListDataBlock<T>::remove(size_t pos)
	{
		fsds::listInternalFunctions::remove(this->m_header, this->m_data, pos);
	}
	template<typename T>
	constexpr void InlineListDataBlock<T>::removeBack()
	{
		fsds::listInternalFunctions::removeBack(this->m_header, this->m_data);
	}
	template<typename T>
	constexpr void InlineListDataBlock<T>::removeFront()
	{
		fsds::listInternalFunctions::removeFront(this->m_header, this->m_data);
	}
	template<typename T>
	constexpr void InlineListDataBlock<T>::removeDeconstruct(size_t pos)
	{
		fsds::listInternalFunctions::removeDeconstruct(this->m_header, this->m_data, pos);
	}
	template<typename T>
	constexpr void InlineListDataBlock<T>::removeBackDeconstruct()
	{
		fsds::listInternalFunctions::removeBackDeconstruct(this->m_header, this->m_data);
	}
	template<typename T>
	constexpr void InlineListDataBlock<T>::removeFrontDeconstruct()
	{
		fsds::listInternalFunctions::removeFrontDeconstruct(this->m_header, this->m_data);
	}
}