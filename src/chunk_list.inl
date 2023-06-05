namespace fsds
{
	template<typename T, size_t chunkSize>
	[[nodiscard]] constexpr  T* ChunkList<T, chunkSize>::add(const T& val)
	{
		if(this->m_chunks.back().sizeInChunk == chunkSize) [[unlikely]]
		{
			this->allocateNewChunk();
		}
		this->m_chunks.back().data[this->m_chunks.back().sizeInChunk] = val;
	}
	template<typename T, size_t chunkSize>
	[[nodiscard]] constexpr T* ChunkList<T, chunkSize>::add(T& val)
	{
		if(this->m_chunks.back().sizeInChunk == chunkSize) [[unlikely]]
		{
			this->allocateNewChunk();
		}
		this->m_chunks.back().data[this->m_chunks.back().sizeInChunk] = val;
	}
	template<typename T, size_t chunkSize>
	constexpr void ChunkList<T, chunkSize>::remove(T* element)
	{
		this->deleteElement(element);
	}
	
	template<typename T, size_t chunkSize>
	[[nodiscard]] constexpr bool ChunkList<T, chunkSize>::isEmpty() const noexcept
	{
		return this->m_chunks.size() == 0;
	}
	template<typename T, size_t chunkSize>
	constexpr size_t ChunkList<T, chunkSize>::size() const noexcept
	{
		size_t total = 0;
		for(size_t i = 0; i < this->m_chunks.size(); i++)
		{
			total += this->m_chunks[i].sizeInChunk;
		}
		return total;
	}

	/*
	template<typename T, size_t chunkSize>
	constexpr void clear()
	{

	}
	*/
	
	template<typename T, size_t chunkSize>
	constexpr ChunkList<T, chunkSize>::Chunk* ChunkList<T, chunkSize>::allocateNewChunk()
	{
		std::allocator<Chunk> alloc;
		ChunkList<T, chunkSize>::Chunk* p = alloc.allocate(1);
		this->m_chunks.append(p);
		return p;
	}
	template<typename T, size_t chunkSize>
	constexpr void ChunkList<T, chunkSize>::deleteElement(T* element)
	{
		for(size_t i = 0; i < this->m_chunks.size(); i++)
		{
			auto offset = element - reinterpret_cast<T*>(this->m_chunks[i].data);
			if((0 <= offset) && (offset < chunkSize))
			{
				this->m_chunks[i].sizeInChunk--;
			}
		}
		if(std::is_trivially_destructible<T>::value)
		{
			element->~T();
		}
	}
}