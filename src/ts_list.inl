namespace fsds
{
	template<typename T, typename Allocator>
	constexpr ts_List<T, Allocator>::ts_List() noexcept(noexcept(Allocator()))
	: m_list(), m_lock()
	{}
	template<typename T, typename Allocator>
	constexpr ts_List<T, Allocator>::ts_List(const size_t& count)
	: m_list(count), m_lock()
	{}
	template<typename T, typename Allocator>
	constexpr ts_List<T, Allocator>::ts_List(const ts_List& other)
	: m_list(other.m_list()), m_lock()
	{}
	template<typename T, typename Allocator>
	constexpr ts_List<T, Allocator>::ts_List(ts_List&& other) noexcept
	: m_list(std::move(other.m_list)), m_lock()
	{}
	template<typename T, typename Allocator>
	constexpr ts_List<T, Allocator>::ts_List(std::initializer_list<T> init, const Allocator& alloc)
	: m_list(init, alloc), m_lock()
	{}
	template<typename T, typename Allocator>
	constexpr ts_List<T, Allocator>::~ts_List()
	{
		//
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
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		return this->m_list[pos];
	}

	template<typename T, typename Allocator>
	constexpr T* ts_List<T, Allocator>::front()
	{
		FTS_READ_WRITE_READ_LOCKGUARD(&this->m_lock);
		return this->m_list.front();
	}
	template<typename T, typename Allocator>
	constexpr T* ts_List<T, Allocator>::back()
	{
		FTS_READ_WRITE_READ_LOCKGUARD(&this->m_lock);
		return this->m_list.back();
	}

	template<typename T, typename Allocator>
	constexpr T* ts_List<T, Allocator>::data()
	{
		FTS_READ_WRITE_READ_LOCKGUARD(&this->m_lock);
		return this->m_list.data();
	}

	template<typename T, typename Allocator>
	[[nodiscard]] constexpr bool ts_List<T, Allocator>::isEmpty() noexcept
	{
		FTS_READ_WRITE_READ_LOCKGUARD(&this->m_lock);
		return this->m_list.isEmpty();
	}
	template<typename T, typename Allocator>
	constexpr size_t ts_List<T, Allocator>::size() noexcept
	{
		FTS_READ_WRITE_READ_LOCKGUARD(&this->m_lock);
		return this->m_list.size();
	}
	template<typename T, typename Allocator>
	constexpr size_t ts_List<T, Allocator>::maxSize() noexcept
	{
		FTS_READ_WRITE_READ_LOCKGUARD(&this->m_lock);
		return this->m_list.maxSize();
	}
	template<typename T, typename Allocator>
	constexpr size_t ts_List<T, Allocator>::capacity() noexcept
	{
		FTS_READ_WRITE_READ_LOCKGUARD(&this->m_lock);
		return this->m_list.capacity();
	}

	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::reserve(size_t newCap)
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.reserve(newCap);
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::clear()
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.clear();
	}
	
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::append(const T& value)
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.append(value);
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void ts_List<T, Allocator>::appendConstruct(Args&&... args)
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.appendConstruct(args...);
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::prepend(const T& value)
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.prepend(value);
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void ts_List<T, Allocator>::prependConstruct(Args&&... args)
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.prependConstruct(args...);
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::insert(size_t pos, const T& value)
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.insert(pos, value);
	}
	template<typename T, typename Allocator>
	template<typename... Args>
	constexpr void ts_List<T, Allocator>::insertConstruct(size_t pos, Args&&... args)
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.insertConstruct(pos, args...);
	}

	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::remove(size_t pos)
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.remove(pos);
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::removeBack()
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.removeBack();
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::removeFront()
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.removeFront();
	}

	template<typename T, typename Allocator>
	constexpr bool ts_List<T, Allocator>::valueEquality(const ts_List<T, Allocator>& other)
	{
		FTS_READ_WRITE_READ_LOCKGUARD_1(&this->m_lock);
		FTS_READ_WRITE_READ_LOCKGUARD_2(&other.m_lock);
		return this->m_list.valueEquality(other);
	}

	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::deepCopy(ts_List<T, Allocator>& dest) const
	{
		FTS_READ_WRITE_READ_LOCKGUARD_1(&this->m_lock);
		FTS_READ_WRITE_WRITE_LOCKGUARD(&dest.m_lock);
		this->m_list.deepCopy(dest);
	}

	template<typename T, typename Allocator>
	constexpr size_t ts_List<T, Allocator>::getExternalReallocateMinimumRequiredSpace()
	{
		FTS_READ_WRITE_READ_LOCKGUARD(&this->m_lock);
		return this->m_list.getExternalReallocateMinimumRequiredSpace();
	}
	template<typename T, typename Allocator>
	constexpr void ts_List<T, Allocator>::externalReallocate(void* ptr, size_t newSizeBytes)
	{
		FTS_READ_WRITE_WRITE_LOCKGUARD(&this->m_lock);
		this->m_list.externalReallocate(ptr, newSizeBytes);
	}

	template<typename T, typename Allocator>
	constexpr bool operator==(const ts_List<T, Allocator>& lhs, const ts_List<T, Allocator>& rhs)
	{
		return lhs.dataReferenceEquality(rhs);
	}
	template<typename T, typename Allocator>
	constexpr bool operator!=(const ts_List<T, Allocator>& lhs, const ts_List<T, Allocator>& rhs)
	{
		return !(lhs == rhs);
	}
}