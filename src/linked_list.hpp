#pragma once

namespace fsds
{
	template<typename T, size_t blockSize, typename Allocator = std::allocator<T>>
	class LinkedListNodeBlock;
	
	template<typename T, size_t blockSize, typename Allocator = std::allocator<T>>
	class LinkedListNode
	{
		public:
			LinkedListNode(T* value, LinkedListNode<T, blockSize, Allocator>* previousNode, LinkedListNode<T, blockSize, Allocator>* nextNode);
			~LinkedListNode();

			constexpr const T* getElement() const;
			constexpr const LinkedListNode<T, blockSize, Allocator>* getPreviousNode() const;
			constexpr void setPreviousNode(LinkedListNode<T, blockSize, Allocator>* newNode);
			constexpr const LinkedListNode<T, blockSize, Allocator>* getNextNode() const;
			constexpr void setNextNode(LinkedListNode<T, blockSize, Allocator>* newNode);
			constexpr void insertNodeBefore(LinkedListNode<T, blockSize, Allocator>* newNode);
			constexpr void insertNodeAfter(LinkedListNode<T, blockSize, Allocator>* newNode);
		
		private:
			T* m_element;
			LinkedListNode<T, blockSize, Allocator>* m_previousNode;
			LinkedListNode<T, blockSize, Allocator>* m_nextNode;

			LinkedListNodeBlock<T, blockSize, Allocator>* parentBlock;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	class LinkedList;

	template<typename T, size_t blockSize, typename Allocator>
	class LinkedListNodeBlock
	{
		public:
			LinkedListNodeBlock();
			~LinkedListNodeBlock();

			LinkedListNode<T, blockSize, Allocator>* getNewNode();
			
			void nodeDestoryedCallback(LinkedListNode<T, blockSize, Allocator>* node);

		private:
			LinkedListNode<T, blockSize> m_data[blockSize];
			size_t m_front;
			size_t m_numNodes;

			LinkedList<T, Allocator>* m_parentList;
	};
	
	template<typename T, typename Allocator>
	class LinkedList{
		public:
			constexpr static size_t blockSize = 16;
			
			constexpr LinkedList() noexcept(noexcept(Allocator()));
			constexpr explicit LinkedList(const Allocator& alloc) noexcept = delete;
			constexpr LinkedList(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			constexpr explicit LinkedList(size_t count, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr LinkedList(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr LinkedList(const LinkedList& other) = delete;
			constexpr LinkedList(const LinkedList& other, const Allocator& alloc) = delete;
			constexpr LinkedList(LinkedList&& other) noexcept = delete;
			constexpr LinkedList(LinkedList&& other, const Allocator& alloc) = delete;
			constexpr LinkedList(std::initializer_list<T> init, const Allocator& alloc = Allocator()) = delete;
			constexpr ~LinkedList();

			constexpr LinkedListNode<T, blockSize, Allocator>* getNewNode();
			
			constexpr LinkedListNode<T, blockSize, Allocator>* first();
			constexpr const LinkedListNode<T, blockSize, Allocator>* first() const;
			constexpr LinkedListNode<T, blockSize, Allocator>* last();
			constexpr const LinkedListNode<T, blockSize, Allocator>* last() const;

			constexpr void clear();

			constexpr size_t size() const;
			constexpr bool isEmpty() const;

			void blockDestoryedCallback(const LinkedListNodeBlock<T, blockSize, Allocator>* block);

		private:
			void allocate();

			T* m_firstNode;
			T* m_lastNode;

			LinkedListNodeBlock<T, blockSize, Allocator>* m_block;
			size_t m_numBlocks;
			size_t m_numNodes;
	};
}

#include "linked_list.inl"