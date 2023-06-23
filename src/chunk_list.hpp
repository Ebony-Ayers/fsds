#ifndef CHUNK_LIST_HPP_HEADER_GUARD
#define CHUNK_LIST_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include "list.hpp"

#include <type_traits>
#include <bitset>

namespace fsds
{
	template<typename T, size_t chunkSize>
	class ChunkList
	{
		public:

			constexpr ChunkList();									//default constructor
			constexpr explicit ChunkList(const size_t& count);		//constructor specifying initial capacity
			constexpr ChunkList(const ChunkList& other);			//copy constructor
			constexpr ChunkList(ChunkList&& other) noexcept;		//move constructor
			constexpr ChunkList(std::initializer_list<T> init) = delete;
			constexpr ~ChunkList();

			constexpr ChunkList& operator=(const ChunkList& other);
			constexpr ChunkList& operator=(ChunkList&& other) noexcept;

			[[nodiscard]] constexpr  T* add(const T& val);
			[[nodiscard]] constexpr T* add(T& val);
			constexpr void remove(T* element);
			
			[[nodiscard]] constexpr bool isEmpty() const noexcept;
			constexpr size_t size() const noexcept;

			constexpr void clear();

		private:
			struct Chunk
			{
				std::bitset<chunkSize> activeFlags;
				T data[chunkSize];
			};

			constexpr Chunk* allocateNewChunk();
			constexpr void DeallocateChunk(size_t index);
			constexpr void DeallocateLastChunk();
			constexpr void deleteElement(T* element);

			fsds::List<Chunk*> m_chunks;
	};
}

#include "chunk_list.inl"

#endif //#ifndef CHUNK_LIST_HPP_HEADER_GUARD