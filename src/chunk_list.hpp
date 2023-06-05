#ifndef CHUNK_LIST_HPP_HEADER_GUARD
#define CHUNK_LIST_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include "list.hpp"

namespace fsds
{
	template<typename T, size_t chunkSize, typename Allocator = std::allocator<T>>
	class ChunkList
	{
		public:

			constexpr ChunkList() noexcept(noexcept(Allocator()));	//default constructor
			constexpr explicit ChunkList(const size_t& count);		//constructor specifying initial capacity
			constexpr ChunkList(const ChunkList& other);			//copy constructor
			constexpr ChunkList(ChunkList&& other) noexcept;		//move constructor
			constexpr ChunkList(std::initializer_list<T> init, const Allocator& alloc = Allocator()) = delete;
			constexpr explicit ChunkList(const Allocator& alloc) noexcept = delete;
			constexpr ChunkList(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr ChunkList(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr ChunkList(const ChunkList& other, const Allocator& alloc) = delete;
			constexpr ChunkList(ChunkList&& other, const Allocator& alloc) = delete;
			constexpr ~ChunkList();

			constexpr ChunkList& operator=(const ChunkList& other);
			constexpr ChunkList& operator=(ChunkList&& other) noexcept;

			constexpr Allocator getAllocator() const noexcept;

			constexpr T& operator[](size_t pos);
			constexpr const T& operator[](size_t pos) const;

			constexpr T& front();
			constexpr const T& front() const;
			constexpr T& back();
			constexpr const T& back() const;

			constexpr T* data();
			constexpr const T* data() const;

			[[nodiscard]] constexpr bool isEmpty() const noexcept;
			constexpr size_t size() const noexcept;
			constexpr size_t maxSize() const noexcept;
			constexpr size_t capacity() const noexcept;

			constexpr void reserve(size_t newCap);
			constexpr void clear();
			
			constexpr void append(const T& value);
			template<typename... Args>
			constexpr void appendConstruct(Args&&... args);
			constexpr void prepend(const T& value);
			template<typename... Args>
			constexpr void prependConstruct(Args&&... args);
			constexpr void insert(size_t pos, const T& value);
			template<typename... Args>
			constexpr void insertConstruct(size_t pos, Args&&... args);

			constexpr void remove(size_t pos);
			constexpr void removeBack();
			constexpr void removeFront();

			constexpr bool valueEquality(const ChunkList<T, chunkSize, Allocator>& other) const;

		private:
			struct Chunk
			{
				size_t sizeInChunk;
				T data[chunkSize];
			};

			fsds::List<T, Allocator> m_list;
	};
}

#include "chunk_list.inl"

#endif //#ifndef CHUNK_LIST_HPP_HEADER_GUARD