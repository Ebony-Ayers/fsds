#ifndef INLINE_LIST_HPP_HEADER_GUARD
#define INLINE_LIST_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include <iterator>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include "list_internal.hpp"
#include "aligned_allocator.hpp"

namespace fsds
{
	template<typename T>
	class InlineListDataBlock
	{
		public:
			constexpr InlineListDataBlock();
			constexpr InlineListDataBlock(const size_t& size, const size_t& capacity, const size_t& front);

			constexpr static size_t calculateRuntimeSizeOf(size_t predictedSize);
			constexpr size_t runtimeSizeOf();
			constexpr size_t runtimeSizeOfDouble();
			template<double factor>
			constexpr size_t runtimeSizeOfFactorIncrease();
			constexpr static size_t calculateNumElementsFromSize(size_t runtimeSize);

			constexpr fsds::listInternalFunctions::ListHeader* getHeader();
			constexpr const fsds::listInternalFunctions::ListHeader* getHeader() const;
			constexpr T* getData();
			constexpr const T* getData() const;
			constexpr T* getUnoffsetedDataPtr();
			constexpr const T* getUnoffsetedDataPtr() const;

			constexpr T& get(size_t pos);
			constexpr const T& get(size_t pos) const;

			constexpr T& front();
			constexpr const T& front() const;
			constexpr T& back();
			constexpr const T& back() const;

			[[nodiscard]] constexpr bool isEmpty() const noexcept;
			constexpr size_t size() const noexcept;
			constexpr size_t maxSize() const noexcept;
			constexpr size_t capacity() const noexcept;
			
			constexpr void safeAppend(const T& value);
			constexpr void copyAppend(const T& value, InlineListDataBlock<T>* newList);
			constexpr void safePrepend(const T& value);
			constexpr void copyPrepend(const T& value, InlineListDataBlock<T>* newList);
			constexpr void safeInsert(size_t pos, const T& value);
			constexpr void copyInsert(size_t pos, const T& value, InlineListDataBlock<T>* newList);
			template<typename... Args>
			constexpr void safeAppendConstruct(Args&&... args);
			template<typename... Args>
			constexpr void copyAppendConstruct(Args&&... args, InlineListDataBlock<T>* newList);
			template<typename... Args>
			constexpr void safePrependConstruct(Args&&... args);
			template<typename... Args>
			constexpr void copyPrependConstruct(Args&&... args, InlineListDataBlock<T>* newList);
			template<typename... Args>
			constexpr void safeInsertConstruct(size_t pos, Args&&... args);
			template<typename... Args>
			constexpr void copyInsertConstruct(size_t pos, Args&&... args, InlineListDataBlock<T>* newList);

			constexpr void remove(size_t pos);
			constexpr void removeBack();
			constexpr void removeFront();
			constexpr void removeWithoutDeconstruct(size_t pos);
			constexpr void removeBackWithoutDeconstruct();
			constexpr void removeFrontWithoutDeconstruct();
		
		private:
			fsds::listInternalFunctions::ListHeader m_header;
			T m_data[1];
	};

	template<typename T, typename Allocator = fsds::AlignedRawAllocator<64>>
	class InlineList
	{
		public:
			constexpr InlineList() noexcept(noexcept(Allocator()));										//default constructor
			constexpr explicit InlineList(const size_t& count);											//constructor specifying initial capacity
			constexpr InlineList(const InlineList& other);												//copy constructor
			constexpr InlineList(InlineList&& other) noexcept;											//move constructor
			constexpr InlineList(std::initializer_list<T> init, const Allocator& alloc = Allocator());	//initializer list
			constexpr explicit InlineList(const Allocator& alloc) noexcept = delete;
			constexpr InlineList(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr InlineList(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr InlineList(const InlineList& other, const Allocator& alloc) = delete;
			constexpr InlineList(InlineList&& other, const Allocator& alloc) = delete;
			constexpr ~InlineList();

			constexpr InlineList& operator=(const InlineList& other);
			constexpr InlineList& operator=(InlineList&& other) noexcept;

			constexpr Allocator getAllocator() const noexcept;

			constexpr T& operator[](size_t pos);
			constexpr const T& operator[](size_t pos) const;

			constexpr T& front();
			constexpr const T& front() const;
			constexpr T& back();
			constexpr const T& back() const;

			constexpr T* data();
			constexpr const T* data() const;
			constexpr InlineListDataBlock<T>* getInlineDataBlock();
			constexpr const InlineListDataBlock<T>* getInlineDataBlock() const;

			[[nodiscard]] constexpr bool isEmpty() const noexcept;
			constexpr size_t size() const noexcept;
			constexpr size_t maxSize() const noexcept;
			constexpr size_t capacity() const noexcept;

			constexpr void reserve(size_t newCap);
			constexpr void clear();
			
			constexpr void append(const T& value);
			constexpr void prepend(const T& value);
			constexpr void insert(size_t pos, const T& value);
			template<typename... Args>
			constexpr void appendConstruct(Args&&... args);
			template<typename... Args>
			constexpr void prependConstruct(Args&&... args);
			template<typename... Args>
			constexpr void insertConstruct(size_t pos, Args&&... args);

			constexpr void remove(size_t pos);
			constexpr void removeBack();
			constexpr void removeFront();
			constexpr void removeWithoutDeconstruct(size_t pos);
			constexpr void removeBackWithoutDeconstruct();
			constexpr void removeFrontWithoutDeconstruct();

			constexpr bool valueEquality(const InlineList<T, Allocator>& other) const;
			
			constexpr void deepCopy(InlineList<T, Allocator>& dest) const;

			//returns the size in bytes
			constexpr size_t getExternalReallocateMinimumRequiredSpace();
			constexpr void externalReallocate(void* ptr, size_t newSizeBytes);
			
		private:
			constexpr static size_t sm_baseAllocation = 16;

			struct allocationByteResult
			{
				InlineListDataBlock<T>* newList;
				size_t numBytesAllocated;
			};
			
			constexpr allocationByteResult allocateBytes(size_t size);
			constexpr InlineListDataBlock<T>* allocateElements(size_t size);
			constexpr void deallocate(InlineListDataBlock<T>* oldList);
			constexpr InlineListDataBlock<T>* reallocate();
			constexpr void deconstructElement(T* element);
			constexpr void deconstructAll();

			constexpr size_t getEffectiveSize() const;

			InlineListDataBlock<T>* m_dataBlock;
	};
}

#include "inline_list_data_block.inl"
#include "inline_list.inl"

#endif //#ifndef INLINE_LIST_HPP_HEADER_GUARD