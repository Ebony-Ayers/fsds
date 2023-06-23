namespace fsds
{
	template<typename T, size_t chunkSize>
	constexpr ChunkList<T, chunkSize>::~ChunkList()
	{
		this->clear();
	}
	
	template<typename T, size_t chunkSize>
	[[nodiscard]] constexpr  T* ChunkList<T, chunkSize>::add(const T& val)
	{
		if(this->m_chunks.back().activeFlags.all()) [[unlikely]]
		{
			this->allocateNewChunk();
		}
		//number of active elements in the last chuck
		size_t activeElementsInChunk = this->m_chunks.back().activeFlags.count();
		this->m_chunks.back().activeFlags[activeElementsInChunk] = true;
		this->m_chunks.back().data[activeElementsInChunk] = val;
	}
	template<typename T, size_t chunkSize>
	[[nodiscard]] constexpr T* ChunkList<T, chunkSize>::add(T& val)
	{
		if(this->m_chunks.back().activeFlags.all()) [[unlikely]]
		{
			this->allocateNewChunk();
		}
		//number of active elements in the last chuck
		size_t activeElementsInChunk = this->m_chunks.back().activeFlags.count();
		this->m_chunks.back().activeFlags[activeElementsInChunk] = true;
		this->m_chunks.back().data[activeElementsInChunk] = val;
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
			total += this->m_chunks[i]->activeFlags.count();
		}
		return total;
	}

	template<typename T, size_t chunkSize>
	constexpr void ChunkList<T, chunkSize>::clear()
	{
		if(std::is_trivially_destructible<T>::value)
		{
			for(size_t i = 0; i < this->m_chunks.size(); i++)
			{
				size_t reverseI = this->m_chunks.size() - i - 1;
				for(size_t j = 0; j < this->m_chunks[reverseI]->activeFlags.size(); j++)
				{
					if(this->m_chunks[reverseI]->activeFlags[j] == true)
					{
						this->m_chunks[reverseI]->data[j].~T();
					}
				}
				this->DeallocateLastChunk();
			}
		}
		else
		{
			for(size_t i = 0; i < this->m_chunks.size(); i++)
			{
				this->DeallocateLastChunk();
			}
		}
	}
	
	template<typename T, size_t chunkSize>
	constexpr ChunkList<T, chunkSize>::Chunk* ChunkList<T, chunkSize>::allocateNewChunk()
	{
		std::allocator<Chunk> alloc;
		ChunkList<T, chunkSize>::Chunk* p = alloc.allocate(1);
		this->m_chunks.append(p);
		p->activeFlags.reset();
		return p;
	}
	template<typename T, size_t chunkSize>
	constexpr void ChunkList<T, chunkSize>::DeallocateChunk(size_t index)
	{
		std::allocator<Chunk> alloc;
		alloc.deallocate(this->m_chunks[index], 1);
		this->m_chunks.remove(index);
	}
	template<typename T, size_t chunkSize>
	constexpr void ChunkList<T, chunkSize>::DeallocateLastChunk()
	{
		std::allocator<Chunk> alloc;
		alloc.deallocate(this->m_chunks.back(), 1);
		this->m_chunks.removeBack();
	}
	template<typename T, size_t chunkSize>
	constexpr void ChunkList<T, chunkSize>::deleteElement(T* element)
	{
		//we have to linearly search though each chunk as they are allocated randomly
		for(size_t i = 0; i < this->m_chunks.size(); i++)
		{
			//we can calculate where the element is in the chunk. If this is the wrong chunk then the offset value will be non-sensicle.
			auto offset = element - reinterpret_cast<T*>(this->m_chunks[i]->data);
			//check if the offset makes sense meaning this is the correct chunk. deallocate the element only if this is true
			if((0 <= offset) && (offset < chunkSize))
			{
				//because elements are not reused then if the element is true then it has not been dellocated allowing is to safely avoid double deallocations.
				//note that if the same memory a chunk occupied was reallocated to the same chunk list then double deallocation would be possible.
				if(this->m_chunks[i]->activeFlags[offset] == true)
				{
					this->m_chunks[i]->activeFlags[offset] = false;
					if(std::is_trivially_destructible<T>::value)
					{
						element->~T();
					}
				}
			}
		}
	}
}