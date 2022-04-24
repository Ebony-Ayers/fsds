namespace fsds
{
	template<typename T, size_t blockSize, typename Allocator>
	MPSCQueue<T, blockSize, Allocator>::MPSCQueue() noexcept(noexcept(Allocator()))
	: m_data(nullptr), m_frontBlockOffset(0), m_firstBlock(0), m_lastBlock(0), m_size(0), m_numBlocks(1), m_capacity(MPSCQueue<T, blockSize, Allocator>::sm_baseAllocation), m_dataAccessLock(0), m_controlBlockLock()
	{
		Allocator alloc;
		this->m_data = alloc.allocate(MPSCQueue<T, blockSize, Allocator>::sm_baseAllocation);
		std::allocator<MPSCQueue<T, blockSize, Allocator>::Block> allocBlock;
		this->m_data[0] = allocBlock.allocate(1);
	}
	template<typename T, size_t blockSize, typename Allocator>
	MPSCQueue<T, blockSize, Allocator>::MPSCQueue(size_t defaultSize, const Allocator& alloc)
	: m_data(nullptr), m_frontBlockOffset(0), m_firstBlock(0), m_lastBlock(0), m_size(0), m_numBlocks(1), m_capacity((defaultSize / blockSize) + 1), m_dataAccessLock(0), m_controlBlockLock()
	{
		this->m_data = alloc.allocate((defaultSize / blockSize) + 1);
		std::allocator<MPSCQueue<T, blockSize, Allocator>::Block> allocBlock;
		this->m_data[0] = allocBlock.allocate(1);
	}
	template<typename T, size_t blockSize, typename Allocator>
	MPSCQueue<T, blockSize, Allocator>::~MPSCQueue()
	{
		if(this->m_data != nullptr)
		{
			Allocator alloc;
			std::allocator<MPSCQueue<T, blockSize, Allocator>::Block> allocBlock;
			for(size_t i = 0; i < this->m_capacity; i++)
			{
				allocBlock.deallocate(this->m_data[i], 1);
			}
			alloc.deallocate(this->m_data, this->m_capacity);
		}
	}

	template<typename T, size_t blockSize, typename Allocator>
	void MPSCQueue<T, blockSize, Allocator>::enqueue(const T& value)
	{
		fts::GenericLockGuard lockGuard(this->m_controlBlockLock);
		
		//test if there is space left in the last block
		size_t lastBlockIndex = this->m_frontBlockOffset + this->m_size;
		
		//move onto the next block if nessesary
		if((lastBlockIndex / blockSize) >= this->m_numBlocks) [[unlikely]]
		{
			//allocate another block
			this->allocateNewBlock();
			lastBlockIndex = 0;
		}
		
		//enqueue the data
		this->m_data[this->m_lastBlock]->data[lastBlockIndex % blockSize] = value;
		this->m_size++;
	}
	template<typename T, size_t blockSize, typename Allocator>
	T MPSCQueue<T, blockSize, Allocator>::dequeue()
	{
		size_t firstBlock;
		size_t frontBlockOffset;
		
		//create a scope for the lock guard to get destoryed in
		{
			fts::GenericLockGuard lockGuard(this->m_controlBlockLock);
			
			//check if there is any data left in this block
			if(this->m_frontBlockOffset >= blockSize) [[unlikely]]
			{
				this->m_frontBlockOffset = 0;

				//advance the block if nessesary
				this->m_firstBlock++;

				//add the amount of counter equal the the amount of data in the block
				if(this->m_size > blockSize) [[likely]]
				{
					this->m_dataAccessLock.addCounter(blockSize);
				}
				else
				{
					this->m_dataAccessLock.addCounter(static_cast<int32_t>(this->m_size));
				}
			}

			//check if there is data to dequeue
			if(this->m_size <= 0) [[unlikely]]
			{
				throw std::out_of_range("No data to dequeue");
			}

			//add countrers to the semaphore if needs be
			//this will happen upon first dequeue and after a clear
			if(this->m_dataAccessLock.numCounters() == 0) [[unlikely]]
			{
				if(this->m_size != 0)
				{
					this->m_dataAccessLock.addCounter(static_cast<int32_t>(blockSize - this->m_frontBlockOffset));
				}
			}
			
			firstBlock = this->m_firstBlock;
			frontBlockOffset = this->m_frontBlockOffset;
			this->m_frontBlockOffset++;
			this->m_size--;
		}
		
		fts::SemaphoreDestoryCounterLockGuard destoryLockGuard(this->m_dataAccessLock);
		//dequeue the data
		return this->m_data[firstBlock]->data[frontBlockOffset];
	}
	template<typename T, size_t blockSize, typename Allocator>
	bool MPSCQueue<T, blockSize, Allocator>::dequeueBlock(Block* dest)
	{
		size_t block;
		bool dataNotAvailable = false;
		size_t copyMode = 0;

		fts::GenericLockGuard lockGuard(this->m_controlBlockLock);

		//check if there are 1, 2 or 3+ blocks
		if(this->m_numBlocks == 0) [[unlikely]]
		{
			dataNotAvailable = true;
		}
		else if(this->m_numBlocks == 1) [[unlikely]]
		{
			//if there is only one block and it is completely full
			if(this->m_size == blockSize)
			{
				std::allocator<MPSCQueue<T, blockSize, Allocator>::Block> allocBlock;
				*dest = this->m_data[this->m_firstBlock];
				this->m_frontBlockOffset = 0;
				this->m_firstBlock = 0;
				this->m_lastBlock = 0;
				this->m_size = 0;
				this->m_data[0] = allocBlock.allocate(1);
				this->m_numBlocks = 1;
			}
			else
			{
				dataNotAvailable = true;
			}
		}
		else if(this->m_numBlocks == 2) [[unlikely]]
		{
			//if there are only two blocks and there is a block that has data
			if(this->m_size >= blockSize) [[unlikely]]
			{
				//if the first block is full and the excess is in the second
				if(this->m_frontBlockOffset == 0) [[likely]]
				{
					*dest = this->m_data[this->m_firstBlock];
					this->m_firstBlock++;
					this->m_frontBlockOffset = 0;
					this->m_size -= blockSize;
					this->m_numBlocks -= 1;
				}
				//if the second block is full and the rest is in the first block
				else if(this->m_size - (blockSize - this->m_frontBlockOffset) == blockSize) [[unlikely]]
				{
					*dest = this->m_data[this->m_firstBlock + 1];
					this->m_lastBlock--;
					this->m_size -= blockSize;
					this->m_numBlocks -= 1;
				}
			}
			else [[likely]]
			{
				dataNotAvailable = true;
			}
		}
		else [[likely]]
		{
			//if there are 3+ blocks return the second
			*dest = this->m_data[this->m_firstBlock + 1];
			this->m_data[this->m_firstBlock + 1] = this->m_data[this->m_firstBlock];
			this->m_firstBlock++;
			this->m_size -= blockSize;
			this->m_numBlocks -= 1;
		}

		fts::SemaphoreDestoryCounterLockGuard destoryLockGuard(this->m_dataAccessLock);
		
		//if no data is available return null
		if(dataNotAvailable)
		{
			*dest = nullptr;
		}
		return dataNotAvailable;
	}
	template<typename T, size_t blockSize, typename Allocator>
	bool MPSCQueue<T, blockSize, Allocator>::tryDequeue(T* dest)
	{
		size_t firstBlock;
		size_t frontBlockOffset;
		
		//create a scope for the lock guard to get destoryed in
		{
			fts::GenericLockGuard lockGuard(this->m_controlBlockLock);
			
			//check if there is any data left in this block
			if(this->m_frontBlockOffset >= blockSize) [[unlikely]]
			{
				this->m_frontBlockOffset = 0;

				//advance the block if nessesary
				this->m_firstBlock++;

				//add the amount of counter equal the the amount of data in the block
				if(this->m_size > blockSize) [[likely]]
				{
					this->m_dataAccessLock.addCounter(blockSize);
				}
				else
				{
					this->m_dataAccessLock.addCounter(static_cast<int32_t>(this->m_size));
				}
			}

			//check if there is data to dequeue
			if(this->m_size <= 0) [[unlikely]]
			{
				return true;
			}

			//add countrers to the semaphore if needs be
			//this will happen upon first dequeue and after a clear
			if(this->m_dataAccessLock.numCounters() == 0) [[unlikely]]
			{
				if(this->m_size != 0)
				{
					this->m_dataAccessLock.addCounter(static_cast<int32_t>(blockSize - this->m_frontBlockOffset));
				}
			}
			
			firstBlock = this->m_firstBlock;
			frontBlockOffset = this->m_frontBlockOffset;
			this->m_frontBlockOffset++;
			this->m_size--;
		}
		
		fts::SemaphoreDestoryCounterLockGuard destoryLockGuard(this->m_dataAccessLock);
		//dequeue the data
		*dest = this->m_data[firstBlock]->data[frontBlockOffset];
		return false;
	}
	template<typename T, size_t blockSize, typename Allocator>
	[[nodiscard]] T& MPSCQueue<T, blockSize, Allocator>::front()
	{
		fts::GenericLockGuard lockGuard(this->m_controlBlockLock);
		
		//check if there is any data left in this block
		if(this->m_frontBlockOffset >= blockSize) [[unlikely]]
		{
			this->m_frontBlockOffset = 0;

			//advance the block if nessesary
			this->m_firstBlock++;
		}
		if(this->m_data == nullptr) [[unlikely]]
		{
			throw std::out_of_range("No data to return");
		}
		if(this->m_size <= 0) [[unlikely]]
		{
			throw std::out_of_range("No data to return");
		}
		return this->m_data[0]->data[this->m_frontBlockOffset];
	}

	template<typename T, size_t blockSize, typename Allocator>
	T& MPSCQueue<T, blockSize, Allocator>::operator[](size_t pos)
	{
		fts::GenericLockGuard lockGuard(this->m_controlBlockLock);
		
		if(pos >= this->m_size) [[unlikely]]
		{
			throw std::out_of_range("Index out of range");
		}
		//double pointer dereference
		//given the position starts inside the first block offset it then work out which block it is in then work out where in the block it is
		size_t absolutePosition = pos + this->m_frontBlockOffset;
		return (this->m_data[(absolutePosition / blockSize) + this->m_firstBlock])->data[absolutePosition % blockSize];
	}

	template<typename T, size_t blockSize, typename Allocator>
	[[nodiscard]] constexpr bool MPSCQueue<T, blockSize, Allocator>::isEmpty() noexcept
	{
		fts::GenericLockGuard lockGuard(this->m_controlBlockLock);

		return this->m_size == 0;
	}
	template<typename T, size_t blockSize, typename Allocator>
	[[nodiscard]] constexpr size_t MPSCQueue<T, blockSize, Allocator>::size() noexcept
	{
		fts::GenericLockGuard lockGuard(this->m_controlBlockLock);
		
		return this->m_size;
	}
	template<typename T, size_t blockSize, typename Allocator>
	[[nodiscard]] constexpr size_t MPSCQueue<T, blockSize, Allocator>::capacity() noexcept
	{
		fts::GenericLockGuard lockGuard(this->m_controlBlockLock);
		
		return this->m_capacity * blockSize;
	}

	template<typename T, size_t blockSize, typename Allocator>
	void MPSCQueue<T, blockSize, Allocator>::reserve(size_t newCap)
	{
		fts::GenericLockGuard lockGuard(this->m_controlBlockLock);
		
		if(newCap > (this->m_capacity * blockSize)) [[likely]]
		{
			this->reallocate(newCap / blockSize);
		}
	}
	template<typename T, size_t blockSize, typename Allocator>
	void MPSCQueue<T, blockSize, Allocator>::clear()
	{
		fts::GenericLockGuard lockGuard(this->m_controlBlockLock);
		
		Allocator alloc;
		std::allocator<MPSCQueue<T, blockSize, Allocator>::Block> allocBlock;
		if(this->m_data != nullptr)
		{
			for(size_t i = 0; i < this->m_capacity; i++)
			{
				allocBlock.deallocate(this->m_data[i], 1);
			}
			alloc.deallocate(this->m_data, this->m_capacity);
		}
		this->m_data = alloc.allocate(MPSCQueue<T, blockSize, Allocator>::sm_baseAllocation);
		this->m_data[0] = allocBlock.allocate(1);
		this->m_frontBlockOffset = 0;
		this->m_firstBlock = 0;
		this->m_lastBlock = 0;
		this->m_size = 0;
		this->m_numBlocks = 1;
		this->m_capacity = MPSCQueue<T, blockSize, Allocator>::sm_baseAllocation;
	}

	template<typename T, size_t blockSize, typename Allocator>
	[[nodiscard]] bool MPSCQueue<T, blockSize, Allocator>::dataReferenceEquality(MPSCQueue<T, blockSize, Allocator>& other)
	{
		fts::GenericLockGuard lockGuard(this->m_controlBlockLock);
		fts::GenericLockGuard lockGuardOther(other.getLock());
		
		return (this->m_data == other.m_data) && (this->m_frontBlockOffset == other.m_frontBlockOffset) && (this->m_firstBlock == other.m_firstBlock) && (this->m_lastBlock == other.m_lastBlock) && (this->m_size == other.m_size);
	}
	template<typename T, size_t blockSize, typename Allocator>
	[[nodiscard]] bool MPSCQueue<T, blockSize, Allocator>::valueEquality(MPSCQueue<T, blockSize, Allocator>& other)
	{
		fts::GenericLockGuard lockGuardThis(this->m_controlBlockLock);
		fts::GenericLockGuard lockGuardOther(other.getLock());
		
		if(this->dataReferenceEqualityNoLock(other))
		{
			return true;
		}
		else
		{
			if(this->m_size == other.m_size)
			{
				for(size_t i = 0; i < this->m_size; i++)
				{
					if(this->atNoLock(i) != other.atNoLock(i))
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

	template<typename T, size_t blockSize, typename Allocator>
	constexpr void MPSCQueue<T, blockSize, Allocator>::reallocate(size_t newSize)
	{
		Allocator alloc;
		auto oldData = this->m_data;
		this->m_data = alloc.allocate(newSize);
		std::copy(oldData + this->m_firstBlock, oldData + this->m_firstBlock + this->m_numBlocks, this->m_data);
		alloc.deallocate(oldData, this->m_capacity);
		this->m_capacity = newSize;
	}
	template<typename T, size_t blockSize, typename Allocator>
	constexpr void MPSCQueue<T, blockSize, Allocator>::allocateNewBlock()
	{
		if(this->m_numBlocks >= this->m_capacity) [[unlikely]]
		{
			this->reallocate(this->m_capacity * 2);
		}
		this->m_numBlocks++;
		this->m_lastBlock++;

		std::allocator<MPSCQueue<T, blockSize, Allocator>::Block> allocBlock;
		this->m_data[this->m_lastBlock] = allocBlock.allocate(1);
	}

	template<typename T, size_t blockSize, typename Allocator>
	constexpr T& MPSCQueue<T, blockSize, Allocator>::atNoLock(const size_t& pos) const
	{
		size_t absolutePosition = pos + this->m_frontBlockOffset;
		return (this->m_data[(absolutePosition / blockSize) + this->m_firstBlock])->data[absolutePosition % blockSize];
	}

	template<typename T, size_t blockSize, typename Allocator>
	constexpr bool MPSCQueue<T, blockSize, Allocator>::dataReferenceEqualityNoLock(MPSCQueue<T, blockSize, Allocator>& other)
	{
		return (this->m_data == other.m_data) && (this->m_frontBlockOffset == other.m_frontBlockOffset) && (this->m_firstBlock == other.m_firstBlock) && (this->m_lastBlock == other.m_lastBlock) && (this->m_size == other.m_size);
	}

	template<typename T, size_t blockSize, typename Allocator>
	fts::SpinLock* MPSCQueue<T, blockSize, Allocator>::getLock()
	{
		return &(this->m_controlBlockLock);
	}



	template<typename T, size_t blockSize = 16, typename Allocator = std::allocator<QueueBlock<T, blockSize>>>
	bool operator==(MPSCQueue<T, blockSize, Allocator>& lhs, MPSCQueue<T, blockSize, Allocator>& rhs)
	{
		return lhs.valueEquality(rhs);
	}
	template<typename T, size_t blockSize = 16, typename Allocator = std::allocator<QueueBlock<T, blockSize>>>
	bool operator!=(MPSCQueue<T, blockSize, Allocator>& lhs, MPSCQueue<T, blockSize, Allocator>& rhs)
	{
		return !lhs.valueEquality(rhs);
	}
}