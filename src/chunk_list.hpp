#ifndef CHUNK_LIST_HPP_HEADER_GUARD
#define CHUNK_LIST_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include "seperate_data_list.hpp"

#include <type_traits>
#include <bitset>
#include <algorithm>

namespace fsds
{
	template<typename T, size_t chunkSize>
	class ChunkList
	{
		public:

			constexpr ChunkList();									//default constructor
			constexpr ChunkList(ChunkList&& other) noexcept;		//move constructor
			constexpr ChunkList(std::initializer_list<T> init) = delete;
			constexpr ~ChunkList();

			constexpr ChunkList& operator=(ChunkList&& other) noexcept;

			[[nodiscard]] constexpr  T* add();
			[[nodiscard]] constexpr  T* add(const T& val);
			[[nodiscard]] constexpr T* add(T& val);
			template<typename... Args>
			[[nodiscard]] constexpr T* addConstruct(Args&&... args);
			constexpr void remove(T* element);
			constexpr void removeDeconstuct(T* element);
			
			[[nodiscard]] constexpr bool isEmpty() const noexcept;
			constexpr size_t size() const noexcept;

			constexpr void clear();

		private:
			struct Chunk
			{
				//the nth activeFlag signifies that the nth element of data is active
				std::bitset<chunkSize> activeFlags;
				T data[chunkSize];
			};

			constexpr Chunk* allocateNewChunk();
			constexpr void DeallocateChunk(size_t index);
			constexpr void DeallocateLastChunk();
			constexpr void deleteElement(T* element);
			constexpr void clearWithoutAllocating();

			fsds::SeperateDataList<Chunk*> m_chunks;
			size_t m_nextElementInChunk;
	};
}

#include "chunk_list.inl"

#endif //#ifndef CHUNK_LIST_HPP_HEADER_GUARD