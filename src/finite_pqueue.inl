namespace fsds
{
	template<typename T, size_t numPriorities, size_t blockSize>
	FinitePQueue<T, numPriorities, blockSize>::FinitePQueue()
	: m_queueHeads(), m_queueTails(), m_queueSizes(), m_availableNodes(), m_rawNodeStorage(), m_totalSize(0)
	{
		//allocate the space for the nodes
		std::allocator<Node> allocNode;
		this->m_rawNodeStorage.append(allocNode.allocate(blockSize));
		
		//put all the nodes into the m_availableNodes linked list linked in sequential order
		for(size_t i = 0; i < blockSize; i++)
		{
			this->m_availableNodes.enqueue(&(this->m_rawNodeStorage.back()[i]));
		}
	}
	template<typename T, size_t numPriorities, size_t blockSize>
	FinitePQueue<T, numPriorities, blockSize>::FinitePQueue(size_t defaultSize)
	: m_queueHeads(), m_queueTails(), m_queueSizes(), m_availableNodes(), m_rawNodeStorage(defaultSize), m_totalSize(0)
	{
		//
		throw;
	}
	template<typename T, size_t numPriorities, size_t blockSize>
	FinitePQueue<T, numPriorities, blockSize>::~FinitePQueue()
	{
		std::allocator<Node> allocNode;
		for(size_t i = 0; i < this->m_rawNodeStorage.size(); i++)
		{
			allocNode.deallocate(this->m_rawNodeStorage[i], blockSize);
		}
	}

	template<typename T, size_t numPriorities, size_t blockSize>
	void FinitePQueue<T, numPriorities, blockSize>::enqueue(const T& value, size_t priority)
	{
		#ifdef FSDS_DEBUG
			if(priority >= numPriorities) [[unlikely]]
			{
				throw std::out_of_range("FinitePQeueue::enqueue called with priority greater than allowed");
			}
		#endif
		
		//get the next available piece of memory
		if(this->m_availableNodes.size() == 0) [[unlikely]]
		{
			this->makeNewBlock();
		}
		Node* node = this->m_availableNodes.dequeue();
		
		//set the values of the node
		node->data = value;
		node->previous = this->m_queueTails[priority];
		node->next = nullptr;

		//update the tails
		//if this priority's queue is empty this node becomes the tail of the queue
		if(this->m_queueTails[priority] == nullptr)
		{
			this->m_queueTails[priority] = node;
		}
		//if there is already data in this priority's queue then set the next node of the current tail to this node and make this node the tail
		else
		{
			this->m_queueTails[priority]->next = node;
			//std::cout << "final " << (this->m_queueTails[priority]->data);
			//std::cout << " -> " << (this->m_queueTails[priority]->next->data) << std::endl;
			this->m_queueTails[priority] = node;
		}
		//update the heads
		//if the priority's queue is empty then this node becomes the head of the queue
		if(this->m_queueHeads[priority] == nullptr)
		{
			this->m_queueHeads[priority] = node;
		}
		//if there is already data in this priority's queue then there is no need to modify the head

		//misc
		this->m_queueSizes[priority]++;
		this->m_totalSize++;
	}
	template<typename T, size_t numPriorities, size_t blockSize>
	T FinitePQueue<T, numPriorities, blockSize>::dequeue(size_t priority)
	{
		#ifdef FSDS_DEBUG
			if(this->m_queueSizes[priority] == 0) [[unlikely]]
			{
				throw std::out_of_range("FinitePQeueue::dequeue tried to dequeue data that didn't exist");
			}
		#endif
		
		//get the node in question
		Node node = *(this->m_queueHeads[priority]);

		//update the heads
		//if this is the last node in this priority's queue then it's next will be null therefore making the entire queue null signifying it is empty
		this->m_queueHeads[priority] = this->m_queueHeads[priority]->next;

		//return the memory to m_availableNodes
		this->m_availableNodes.enqueue(this->m_queueHeads[priority]);

		//misc
		this->m_queueSizes[priority]--;
		this->m_totalSize--;

		//return the value
		return node.data;
	}

	template<typename T, size_t numPriorities, size_t blockSize>
	bool FinitePQueue<T, numPriorities, blockSize>::tryDequeue(T* dest, size_t priority)
	{
		#ifdef FSDS_DEBUG
			if(dest == nullptr) [[unlikely]]
			{
				throw std::invalid_argument("FinitePQeueue::tryDequeue called with nullptr");
			}
		#endif
		
		if(this->m_queueSizes[priority] > 0)
		{
			//get the node in question
			Node node = *(this->m_queueHeads[priority]);

			//update the heads
			//if this is the last node in this priority's queue then it's next will be null therefore making the entire queue null signifying it is empty
			this->m_queueHeads[priority] = this->m_queueHeads[priority]->next;

			//return the memory to m_availableNodes
			this->m_availableNodes.enqueue(this->m_queueHeads[priority]);

			//misc
			this->m_queueSizes[priority]--;
			this->m_totalSize--;

			//return the value
			*dest = node.data;
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename T, size_t numPriorities, size_t blockSize>
	[[nodiscard]] T& FinitePQueue<T, numPriorities, blockSize>::front(size_t priority) const
	{
		#ifdef FSDS_DEBUG
			if(priority >= numPriorities) [[unlikely]]
			{
				throw std::out_of_range("FinitePQeueue::front called with priority greater than allowed");
			}
			if(this->m_queueSizes[priority] == 0) [[unlikely]]
			{
				throw std::out_of_range("FinitePQeueue::front tried to access data that didn't exist");
			}
		#endif
		
		return this->m_queueHeads[priority]->data;
	}

	template<typename T, size_t numPriorities, size_t blockSize>
	[[nodiscard]] constexpr bool FinitePQueue<T, numPriorities, blockSize>::isEmpty() const noexcept
	{
		return (this->m_totalSize == 0);
	}
	template<typename T, size_t numPriorities, size_t blockSize>
	[[nodiscard]] constexpr size_t FinitePQueue<T, numPriorities, blockSize>::size() const noexcept
	{
		return this->m_totalSize;
	}
	template<typename T, size_t numPriorities, size_t blockSize>
	[[nodiscard]] constexpr size_t FinitePQueue<T, numPriorities, blockSize>::capacity() const noexcept
	{
		return this->m_rawNodeStorage.size() * blockSize;
	}

	template<typename T, size_t numPriorities, size_t blockSize>
	void FinitePQueue<T, numPriorities, blockSize>::reserve(size_t newCap)
	{
		//
	}
	template<typename T, size_t numPriorities, size_t blockSize>
	void FinitePQueue<T, numPriorities, blockSize>::clear()
	{
		//destructor
		std::allocator<Node> allocNode;
		for(size_t i = 0; i < this->m_rawNodeStorage.size(); i++)
		{
			allocNode.deallocate(this->m_rawNodeStorage[i], blockSize);
		}
		
		//member initialiser list
		this->m_queueHeads = std::array<Node*, numPriorities>();
		this->m_queueTails = std::array<Node*, numPriorities>();
		this->m_queueSizes = std::array<size_t, numPriorities>();
		this->m_availableNodes = fsds::SPSCQueue<Node*>(1);
		this->m_rawNodeStorage = fsds::List<Node*>();
		this->m_totalSize = 0;

		//constructor
		//allocate the space for the nodes
		this->m_rawNodeStorage.append(allocNode.allocate(blockSize));
		
		//put all the nodes into the m_availableNodes linked list linked in sequential order
		for(size_t i = 0; i < blockSize; i++)
		{
			this->m_availableNodes.enqueue(&(this->m_rawNodeStorage.back()[i]));
		}
	}

	template<typename T, size_t numPriorities, size_t blockSize>
	void FinitePQueue<T, numPriorities, blockSize>::makeNewBlock()
	{
		//allocate the space for the new block
		std::allocator<Node> allocNode;
		
		Node* temp = allocNode.allocate(blockSize);
		this->m_rawNodeStorage.append(temp);
		
		//put all the nodes into the m_availableNodes linked list linked in sequential order
		for(size_t i = 0; i < blockSize; i++)
		{
			this->m_availableNodes.enqueue(&(this->m_rawNodeStorage.back()[i]));
		}
	}
}