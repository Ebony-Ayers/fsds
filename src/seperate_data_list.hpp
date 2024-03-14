#ifndef SEPERATE_DATA_LIST_HPP_HEADER_GUARD
#define SEPERATE_DATA_LIST_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include <iterator>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include "list_internal.hpp"

namespace fsds
{
	template<typename T, typename Allocator = std::allocator<T>>
	class SeperateDataList
	{
		public:
			constexpr SeperateDataList() noexcept(noexcept(Allocator()));										//default constructor
			constexpr explicit SeperateDataList(const size_t& count);											//constructor specifying initial capacity
			constexpr SeperateDataList(const SeperateDataList& other);											//copy constructor
			constexpr SeperateDataList(SeperateDataList&& other) noexcept;										//move constructor
			constexpr SeperateDataList(std::initializer_list<T> init, const Allocator& alloc = Allocator());	//initializer list
			constexpr explicit SeperateDataList(const Allocator& alloc) noexcept = delete;
			constexpr SeperateDataList(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr SeperateDataList(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr SeperateDataList(const SeperateDataList& other, const Allocator& alloc) = delete;
			constexpr SeperateDataList(SeperateDataList&& other, const Allocator& alloc) = delete;
			constexpr ~SeperateDataList();

			constexpr SeperateDataList& operator=(const SeperateDataList& other);
			constexpr SeperateDataList& operator=(SeperateDataList&& other) noexcept;

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
			constexpr void removeDeconstruct(size_t pos);
			constexpr void removeBackDeconstruct();
			constexpr void removeFrontDeconstruct();

			constexpr bool valueEquality(const SeperateDataList<T, Allocator>& other) const;

			constexpr void deepCopy(SeperateDataList<T, Allocator>& dest) const;

			//returns the size in bytes
			constexpr size_t getExternalReallocateMinimumRequiredSpace();
			constexpr void externalReallocate(void* ptr, size_t newSizeBytes);
		
		private:
			constexpr static size_t sm_baseAllocation = 16;
			
			void reallocate(size_t newSize);
			constexpr void deallocate();
			constexpr void deconstructElement(T* element);
			constexpr void deconstructAll();

			T* m_data;
			fsds::listInternalFunctions::ListHeader m_header;
			//size_t m_size;
			//size_t m_capacity;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(const SeperateDataList<T, Allocator>& lhs, const SeperateDataList<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator!=(const SeperateDataList<T, Allocator>& lhs, const SeperateDataList<T, Allocator>& rhs);

	template<typename T, typename Allocator = std::allocator<T>>
	class ts_SeperateDataList
	{
		public:
			constexpr ts_SeperateDataList() noexcept(noexcept(Allocator()));	//default constructor
			constexpr explicit ts_SeperateDataList(const size_t& count);		//constructor specifying initial capacity
			constexpr ts_SeperateDataList(const ts_SeperateDataList& other);					//copy constructor
			constexpr ts_SeperateDataList(ts_SeperateDataList&& other) noexcept;				//move constructor
			constexpr ts_SeperateDataList(std::initializer_list<T> init, const Allocator& alloc = Allocator());	//initializer list
			constexpr explicit ts_SeperateDataList(const Allocator& alloc) noexcept = delete;
			constexpr ts_SeperateDataList(size_t count, const T& value, const Allocator& alloc = Allocator()) = delete;
			template<typename InputIt>
			constexpr ts_SeperateDataList(InputIt first, InputIt last, const Allocator& alloc = Allocator()) = delete;
			constexpr ts_SeperateDataList(const ts_SeperateDataList& other, const Allocator& alloc) = delete;
			constexpr ts_SeperateDataList(ts_SeperateDataList&& other, const Allocator& alloc) = delete;
			constexpr ~ts_SeperateDataList();

			constexpr Allocator getAllocator() const noexcept;

			constexpr T* operator[](size_t pos);

			constexpr T* front();
			constexpr T* back();

			constexpr T* data();

			[[nodiscard]] constexpr bool isEmpty() noexcept;
			constexpr size_t size() noexcept;
			constexpr size_t maxSize() noexcept;
			constexpr size_t capacity() noexcept;

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

			constexpr bool valueEquality(const ts_SeperateDataList<T, Allocator>& other);

			constexpr void deepCopy(ts_SeperateDataList<T, Allocator>& dest) const;

			//returns the size in bytes
			constexpr size_t getExternalReallocateMinimumRequiredSpace();
			constexpr void externalReallocate(void* ptr, size_t newSizeBytes);
		
		private:
			fsds::SeperateDataList<T,Allocator> m_list;
			fts::ReadWriteLock m_lock;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator==(const ts_SeperateDataList<T, Allocator>& lhs, const ts_SeperateDataList<T, Allocator>& rhs);
	template<typename T, typename Allocator = std::allocator<T>>
	constexpr bool operator!=(const ts_SeperateDataList<T, Allocator>& lhs, const ts_SeperateDataList<T, Allocator>& rhs);
}

#include "seperate_data_list.inl"
#include "ts_seperate_data_list.inl"

#endif //#ifndef SEPERATE_DATA_LIST_HPP_HEADER_GUARD