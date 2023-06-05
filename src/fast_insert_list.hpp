#ifndef FAST_INSERT_LIST_HPP_HEADER_GUARD
#define FAST_INSERT_LIST_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include "list.hpp"

namespace fsds
{
	template<typename T>
	class FastInsertListDiscontiguousIterator;
	template<typename T>
	class FastInsertListContiguousIterator;

	template<typename T>
	class FastInsertList
	{
		public:
			constexpr FastInsertList() noexcept;									//default constructor
			constexpr explicit FastInsertList(const size_t& count);					//constructor specifying initial capacity
			constexpr FastInsertList(const FastInsertList& other);					//copy constructor
			constexpr FastInsertList(FastInsertList&& other) noexcept;				//move constructor
			constexpr FastInsertList(std::initializer_list<T> init) = delete;
			constexpr ~FastInsertList();

			constexpr FastInsertList& operator=(const FastInsertList& other);
			constexpr FastInsertList& operator=(FastInsertList&& other) noexcept;

			constexpr T& operator[](size_t pos);
			constexpr const T& operator[](size_t pos) const;

			[[nodiscard]] constexpr bool isEmpty() const noexcept;
			constexpr size_t size() const noexcept;
			constexpr size_t capacity() const noexcept;

			constexpr void reserve(size_t newCap);
			constexpr void clear();
			
			constexpr size_t add(const T& value);
			constexpr size_t add(T&& value);
			template<typename... Args>
			constexpr size_t addConstruct(Args&&... args);

			constexpr void remove(size_t pos);

			constexpr bool valueEquality(const FastInsertList<T>& other) const;

			constexpr void deepCopy(FastInsertList<T>& dest) const;

			[[nodiscard]] constexpr FastInsertListContiguousIterator<T> getIterator() const;

		private:
			union ListElement;
			struct LinkedListNode
			{
				size_t nextData;
				ListElement* nextAvailable;
			};
			union ListElement
			{
				T data;
				LinkedListNode next;
			};
			fsds::List<ListElement> m_rawList;

			ListElement* m_nextAvailable;

			friend class FastInsertListDiscontiguousIterator<T>;
			friend class FastInsertListContiguousIterator<T>;
	};

	template<typename T>
	class FastInsertListDiscontiguousIterator
	{
		public:
			constexpr FastInsertListDiscontiguousIterator(const FastInsertList<T>& l);

			[[nodiscard]] constexpr T& get(FastInsertList<T>& l) const;
			[[nodiscard]] constexpr const T& getConst(const FastInsertList<T>& l) const;
			
			constexpr T& next(FastInsertList<T>& l);
			constexpr const T& nextConst(const FastInsertList<T>& l);

			[[nodiscard]] constexpr bool notDone(const FastInsertList<T>& l) const;
		
		private:
			size_t m_index;
			FastInsertList<T>::ListElement* m_nextAvailable;
	};

	template<typename T>
	class FastInsertListContiguousIterator
	{
		public:
			constexpr FastInsertListContiguousIterator(const FastInsertList<T>& l);

			[[nodiscard]] constexpr T& get(FastInsertList<T>& l) const;
			[[nodiscard]] constexpr const T& getConst(const FastInsertList<T>& l) const;
			
			constexpr T& next(FastInsertList<T>& l);
			constexpr const T& nextConst(const FastInsertList<T>& l);

			[[nodiscard]] constexpr bool notDone() const;
			[[nodiscard]] constexpr FastInsertListDiscontiguousIterator<T> getDiscontigousIterator(const FastInsertList<T>& l) const;
			[[nodiscard]] constexpr bool shouldGetDiscontigousIterator(const FastInsertList<T>& l) const;

		private:
			size_t m_index;
			size_t m_finish;
	};
}


#include "fast_insert_list.inl"

#endif //#ifndef FAST_INSERT_LIST_HPP_HEADER_GUARD