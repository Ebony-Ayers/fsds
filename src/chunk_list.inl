namespace fsds
{
	template<typename T, size_t chunkSize>
	constexpr ChunkList<T, chunkSize>::ChunkList()
	: m_chunks(), m_nextElementInChunk(0)
	{
		this->allocateNewChunk();
	}

	template<typename T, size_t chunkSize>
	constexpr ChunkList<T, chunkSize>::ChunkList(ChunkList&& other) noexcept
	{
		if(!this->isEmpty)
		{
			this->clearWithoutAllocating();
		}
		m_chunks(std::move(other.m_chunks));
		this->m_nextElementInChunk = other.m_nextElementInChunk;
	}
	
	template<typename T, size_t chunkSize>
	constexpr ChunkList<T, chunkSize>::~ChunkList()
	{
		this->clearWithoutAllocating();
	}

	template<typename T, size_t chunkSize>
	constexpr ChunkList<T, chunkSize>& ChunkList<T, chunkSize>::operator=(ChunkList&& other) noexcept
	{
		if(!this->isEmpty)
		{
			this->clearWithoutAllocating();
		}
		m_chunks(std::move(other.m_chunks));
		this->m_nextElementInChunk = other.m_nextElementInChunk;
	}
	

	template<typename T, size_t chunkSize>
	[[nodiscard]] constexpr  T* ChunkList<T, chunkSize>::add()
	{
		//if the last element in the chunk has been used move onto the next chunk
		if(this->m_nextElementInChunk >= chunkSize) [[unlikely]]
		{
			this->allocateNewChunk();
		}
		//mark the peice of memory as active
		this->m_chunks.back()->activeFlags[this->m_nextElementInChunk] = true;
		//get a reference to the new memory
		T* newElement = &(this->m_chunks.back()->data[this->m_nextElementInChunk]);
		this->m_nextElementInChunk++;
		return newElement;
	}
	template<typename T, size_t chunkSize>
	[[nodiscard]] constexpr  T* ChunkList<T, chunkSize>::add(const T& val)
	{
		//if the last element in the chunk has been used move onto the next chunk
		if(this->m_nextElementInChunk >= chunkSize) [[unlikely]]
		{
			this->allocateNewChunk();
		}
		//mark the peice of memory as active
		this->m_chunks.back()->activeFlags[this->m_nextElementInChunk] = true;
		//get a reference to the new memory
		T* newElement = &(this->m_chunks.back()->data[this->m_nextElementInChunk]);
		*newElement = val;
		this->m_nextElementInChunk++;
		return newElement;
	}
	template<typename T, size_t chunkSize>
	[[nodiscard]] constexpr T* ChunkList<T, chunkSize>::add(T& val)
	{
		//if the last element in the chunk has been used move onto the next chunk
		if(this->m_nextElementInChunk >= chunkSize) [[unlikely]]
		{
			this->allocateNewChunk();
		}
		//mark the peice of memory as active
		this->m_chunks.back()->activeFlags[this->m_nextElementInChunk] = true;
		//get a reference to the new memory
		T* newElement = &(this->m_chunks.back()->data[this->m_nextElementInChunk]);
		*newElement = val;
		this->m_nextElementInChunk++;
		return m_nextElementInChunk;
	}
	template<typename T, size_t chunkSize>
	constexpr void ChunkList<T, chunkSize>::remove(T* element)
	{
		this->deleteElement(element);
	}
	
	template<typename T, size_t chunkSize>
	[[nodiscard]] constexpr bool ChunkList<T, chunkSize>::isEmpty() const noexcept
	{
		return this->size() == 0;
	}
	template<typename T, size_t chunkSize>
	constexpr size_t ChunkList<T, chunkSize>::size() const noexcept
	{
		size_t total = 0;
		for(size_t i = 0; i < this->m_chunks.size(); i++)
		{
			//count the number of active elements in the ith chunk
			total += this->m_chunks[i]->activeFlags.count();
		}
		return total;
	}

	template<typename T, size_t chunkSize>
	constexpr void ChunkList<T, chunkSize>::clear()
	{
		this->clearWithoutAllocating();
		this->allocateNewChunk();
	}
	
	template<typename T, size_t chunkSize>
	constexpr ChunkList<T, chunkSize>::Chunk* ChunkList<T, chunkSize>::allocateNewChunk()
	{
		std::allocator<Chunk> alloc;
		ChunkList<T, chunkSize>::Chunk* p = alloc.allocate(1);
		this->m_chunks.append(p);
		p->activeFlags.reset();
		this->m_nextElementInChunk = 0;
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
			ptrdiff_t offset = element - reinterpret_cast<T*>(this->m_chunks[i]->data);
			//check if the offset makes sense meaning this is the correct chunk. deallocate the element only if this is true
			if((0 <= offset) && (offset < static_cast<ptrdiff_t>(chunkSize)))
			{
				//because elements are not reused then if the element is true then it has not been dellocated allowing is to safely avoid double deallocations.
				//note that if the same memory a chunk occupied was reallocated to the same chunk list then double deallocation would be possible.
				if(this->m_chunks[i]->activeFlags[static_cast<size_t>(offset)] == true)
				{
					this->m_chunks[i]->activeFlags[static_cast<size_t>(offset)] = false;
					if(std::is_trivially_destructible<T>::value)
					{
						element->~T();
					}
				}
			}
		}
	}
	template<typename T, size_t chunkSize>
	constexpr void ChunkList<T, chunkSize>::clearWithoutAllocating()
	{
		if(std::is_trivially_destructible<T>::value)
		{
			for(size_t i = 0; i < this->m_chunks.size(); i++)
			{
				//deallocating chunks in reverse is faster as the chunks are stored in a list
				size_t reverseI = this->m_chunks.size() - i - 1;
				for(size_t j = 0; j < this->m_chunks[reverseI]->activeFlags.size(); j++)
				{
					//only call the destructor if the object is active
					if(this->m_chunks[reverseI]->activeFlags[j] == true)
					{
						this->m_chunks[reverseI]->data[j].~T();
					}
				}
				//reset the flags to avoid potential future memory issues if this chunk was to be reused.
				this->m_chunks.back()->activeFlags.reset();
				this->DeallocateLastChunk();
			}
		}
		else
		{
			//deallocating chunks in reverse is faster as the chunks are stored in a list
			for(size_t i = 0; i < this->m_chunks.size(); i++)
			{
				//reset the flags to avoid potential future memory issues if this chunk was to be reused.
				this->m_chunks.back()->activeFlags.reset();
				this->DeallocateLastChunk();
			}
		}
	}
}