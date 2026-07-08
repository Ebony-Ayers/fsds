#pragma once
#ifndef STABLE_LIST_HPP_HEADER_GUARD
#define STABLE_LIST_HPP_HEADER_GUARD
#include "../fsds_options.hpp"
#include "../pch.hpp"

#include "stable_list.h"

#include <iterator>
#include <limits>
#include <memory>
#include <utility>

//cache line size (x86)
#define FSDS_STABLE_LIST_CACHE_LINE_SIZE_BYTES 64
//how many elements to allocate default
#define FSDS_STABLE_LIST_DEFAULT_ALLOCATION_SIZE 16
//how many elements to put at the start of the list be default
#define FSDS_STABLE_LIST_DEFAULT_FRONT_OFFSET 4
//when reallocating if there are less then threshold elements at the other end of the list to that which is being reallocated reallocate both ends
#define FSDS_STABLE_LIST_DOUBLE_REALLOCATION_THRESHOLD 4

namespace fsds
{
    //StableList is a type similar to std::vector except it provides iRefs and ascociated benefits. As a compromise random insertion and deletion is not supported.
    template<typename T, typename Allocator = std::allocator<T>>
    class StableList
    {
        public:
            //an iRef is a special type which behavies like a hybrid of a pointer and an index with trivial overhead.
            //Like a pointer an iRef will still point to same element even after the front of a list is mofidied.
            //Like an index an iRef will still point to the same element even after the list performs an internal reallocation.
            //Additionally iRefs provide a probalistic way to catch passing it to the wrong list instnace.
            //This is limited to lists of ~4.3 billion elements with no more than ~4.3 billion instances of lists.
            //Escedeing these restrictions will result in comprosimed error checking were some errors may not be caught.
            //Note that traditional bounds checking is uneffected size and number of lists.
            struct IRef
            {
                size_t value;

                explicit constexpr IRef(size_t v) : value(v) {}
                constexpr IRef() : value(0) {}

                constexpr bool operator==(const IRef& other) const = default;
                constexpr auto operator<=>(const IRef& other) const = default;
            };

            class ConstIterator;

            class Iterator
            {
                public:
                    using iterator_category = std::random_access_iterator_tag;
                    using value_type = T;
                    using difference_type = std::ptrdiff_t;
                    using pointer = T*;
                    using reference = T&;

                    constexpr Iterator() noexcept;
                    constexpr Iterator(IRef iRef, StableList* list, int step) noexcept;

                    [[nodiscard]] constexpr reference operator*() const;
                    [[nodiscard]] constexpr pointer operator->() const;
                    [[nodiscard]] constexpr reference operator[](difference_type n) const;

                    constexpr Iterator& operator++() noexcept;
                    constexpr Iterator operator++(int) noexcept;
                    constexpr Iterator& operator--() noexcept;
                    constexpr Iterator operator--(int) noexcept;

                    constexpr Iterator& operator+=(difference_type n) noexcept;
                    constexpr Iterator& operator-=(difference_type n) noexcept;

                    [[nodiscard]] constexpr Iterator operator+(difference_type n) const noexcept;
                    [[nodiscard]] constexpr Iterator operator-(difference_type n) const noexcept;
                    [[nodiscard]] friend constexpr Iterator operator+(difference_type n, const Iterator& it) noexcept { return it + n; }

                    [[nodiscard]] constexpr difference_type operator-(const Iterator& other) const noexcept;

                    [[nodiscard]] constexpr bool operator==(const Iterator& other) const noexcept;
                    [[nodiscard]] constexpr bool operator!=(const Iterator& other) const noexcept;
                    [[nodiscard]] constexpr bool operator<(const Iterator& other) const noexcept;
                    [[nodiscard]] constexpr bool operator>(const Iterator& other) const noexcept;
                    [[nodiscard]] constexpr bool operator<=(const Iterator& other) const noexcept;
                    [[nodiscard]] constexpr bool operator>=(const Iterator& other) const noexcept;

                    [[nodiscard]] constexpr IRef iRef() const noexcept;
                    [[nodiscard]] constexpr int step() const noexcept;

                    friend class ConstIterator;

                private:
                    IRef m_iRef;
                    StableList* m_list;
                    int m_step;
            };

            class ConstIterator
            {
                public:
                    using iterator_category = std::random_access_iterator_tag;
                    using value_type = T;
                    using difference_type = std::ptrdiff_t;
                    using pointer = const T*;
                    using reference = const T&;

                    constexpr ConstIterator() noexcept;
                    constexpr ConstIterator(IRef iRef, const StableList* list, int step) noexcept;
                    constexpr ConstIterator(const Iterator& it) noexcept;

                    [[nodiscard]] constexpr reference operator*() const;
                    [[nodiscard]] constexpr pointer operator->() const;
                    [[nodiscard]] constexpr reference operator[](difference_type n) const;

                    constexpr ConstIterator& operator++() noexcept;
                    constexpr ConstIterator operator++(int) noexcept;
                    constexpr ConstIterator& operator--() noexcept;
                    constexpr ConstIterator operator--(int) noexcept;

                    constexpr ConstIterator& operator+=(difference_type n) noexcept;
                    constexpr ConstIterator& operator-=(difference_type n) noexcept;

                    [[nodiscard]] constexpr ConstIterator operator+(difference_type n) const noexcept;
                    [[nodiscard]] constexpr ConstIterator operator-(difference_type n) const noexcept;
                    [[nodiscard]] friend constexpr ConstIterator operator+(difference_type n, const ConstIterator& it) noexcept { return it + n; }

                    [[nodiscard]] constexpr difference_type operator-(const ConstIterator& other) const noexcept;

                    [[nodiscard]] constexpr bool operator==(const ConstIterator& other) const noexcept;
                    [[nodiscard]] constexpr bool operator!=(const ConstIterator& other) const noexcept;
                    [[nodiscard]] constexpr bool operator<(const ConstIterator& other) const noexcept;
                    [[nodiscard]] constexpr bool operator>(const ConstIterator& other) const noexcept;
                    [[nodiscard]] constexpr bool operator<=(const ConstIterator& other) const noexcept;
                    [[nodiscard]] constexpr bool operator>=(const ConstIterator& other) const noexcept;

                    [[nodiscard]] constexpr IRef iRef() const noexcept;
                    [[nodiscard]] constexpr int step() const noexcept;

                private:
                    IRef m_iRef;
                    const StableList* m_list;
                    int m_step;
            };

            constexpr StableList();                                                                             //default constructor
            constexpr explicit StableList(const Allocator& alloc);                                              //default constructor (with allocator)
            constexpr explicit StableList(size_t count, const Allocator& alloc = Allocator());                  //starting size constructor
            constexpr explicit StableList(size_t count, const T& value, const Allocator& alloc = Allocator());  //fill constructor
            constexpr StableList(const StableList& other);                                                      //copy constructor
            constexpr StableList(const StableList& other, const Allocator& alloc);                              //copy constructor (with allocator)
            constexpr StableList(StableList&& other);                                                           //move constructor
            constexpr StableList(StableList&& other, const Allocator& alloc);                                   //move constructor (with allocator)
            template<typename InputIt>
            constexpr StableList(InputIt first, InputIt last, const Allocator& alloc = Allocator());            //iterator constructor
            constexpr StableList(std::initializer_list<T> init, const Allocator& alloc = Allocator());          //initialiser list constructor
            constexpr ~StableList();                                                                            //destructor

            constexpr StableList& operator=(const StableList& other);          //copy assign
			constexpr StableList& operator=(StableList&& other) noexcept;      //move assign
            constexpr StableList& operator=(std::initializer_list<T> ilist);   //initialiser list assign

            constexpr void assign(size_t count, const T& value);               //fill assign
            template<typename InputIt>
            constexpr void assign(InputIt first, InputIt last);                //iterator assign
            constexpr void assign(std::initializer_list<T> ilist);             //initialiser list assign

            [[nodiscard]] constexpr Allocator get_allocator() const;

            [[nodiscard]] constexpr T& at(size_t index);
            [[nodiscard]] constexpr const T& at(size_t index) const;
            [[nodiscard]] constexpr T& at(const IRef& iRef);
            [[nodiscard]] constexpr const T& at(const IRef& iRef) const;

            //unlike std::vector all operator[] perform bounds checking
            [[nodiscard]] constexpr T& operator[](size_t index);
            [[nodiscard]] constexpr const T& operator[](size_t index) const;
            [[nodiscard]] constexpr T& operator[](const IRef& iRef);
            [[nodiscard]] constexpr const T& operator[](const IRef& iRef) const;

            [[nodiscard]] constexpr T& front();
            [[nodiscard]] constexpr const T& front() const;
            [[nodiscard]] constexpr IRef frontIRef() const;
            [[nodiscard]] constexpr T& back();
            [[nodiscard]] constexpr const T& back() const;
            [[nodiscard]] constexpr IRef backIRef() const;

            [[nodiscard]] constexpr T* data();
            [[nodiscard]] constexpr const T* data() const;

            [[nodiscard]] Iterator begin();
            [[nodiscard]] ConstIterator begin() const;
            [[nodiscard]] ConstIterator cbegin() const;
            [[nodiscard]] Iterator end();
            [[nodiscard]] ConstIterator end() const;
            [[nodiscard]] ConstIterator cend() const;
            [[nodiscard]] Iterator rbegin();
            [[nodiscard]] ConstIterator rbegin() const;
            [[nodiscard]] ConstIterator rcbegin() const;
            [[nodiscard]] Iterator rend();
            [[nodiscard]] ConstIterator rend() const;
            [[nodiscard]] ConstIterator rcend() const;
            //a negative step represents a reverse iterator
            [[nodiscard]] Iterator getIterator(const IRef& start, const int step);
            //a negative step represents a reverse iterator
            [[nodiscard]] ConstIterator getConstIterator(const IRef& start, const int step) const;
            //an unordered iterator is will return the fastest possible iterater. Relying on the order is undefined behavior
            [[nodiscard]] Iterator getUnorderedIterator();
            //an unordered iterator is will return the fastest possible iterater. Relying on the order is undefined behavior
            [[nodiscard]] ConstIterator getUnorderedConstIterator() const;

            [[nodiscard]] constexpr bool empty() const noexcept;
            [[nodiscard]] constexpr bool isEmpty() const noexcept;
			[[nodiscard]] constexpr size_t size() const noexcept;
			[[nodiscard]] constexpr size_t maxSize() const noexcept;
			[[nodiscard]] constexpr size_t capacity() const noexcept;

            //converting iRefs and indices is restricted to the list instance that created the iRef.
            //Doing so on a different instance is undefined behavior however will often trigger an error
            [[nodiscard]] constexpr size_t iRefToIndex(const IRef& iRef);
            //converting an index to an iRef must be performed on the instance of the list the index is supposed to access.
            //Doing so on a difference instance is a logic error may or may not be automatically caught.
            [[nodiscard]] constexpr IRef indexToIRef(const size_t& index);

            constexpr void reserve(size_t newCap);
            constexpr void shrinkToFit();
            constexpr void shrink_to_fit();
			constexpr void clear();

            constexpr void append(const T& value);
			constexpr void prepend(const T& value);
			template<typename... Args>
			constexpr void appendConstruct(Args&&... args);
			template<typename... Args>
			constexpr void prependConstruct(Args&&... args);
            constexpr void push_back(const T& value);
            constexpr void push_front(const T& value);
            template<typename... Args>
            constexpr void emplace_back(Args&&... args);
            template<typename... Args>
            constexpr void emplace_front(Args&&... args);

            constexpr void concatenateBack(const StableList& other);
            constexpr void concatenateFront(const StableList& other);
			
			constexpr void removeBack();
			constexpr void removeFront();
			constexpr void removeBackWithoutDeconstruct();
			constexpr void removeFrontWithoutDeconstruct();
            constexpr void pop_back();
            constexpr void pop_front();

            //this data stracture does not allow random insertion and removal however to maintain combatibility with the interface of std::vector they have been deleted
            constexpr Iterator insert(ConstIterator pos, const T& value) = delete;
            constexpr Iterator insert(ConstIterator pos, T&& value) = delete;
            constexpr Iterator insert(ConstIterator pos, size_t count, const T& value) = delete;
            template<typename InputIt>
            constexpr Iterator insert(ConstIterator pos, InputIt first, InputIt last) = delete;
            constexpr Iterator insert(ConstIterator pos, std::initializer_list<T> ilist) = delete;
            template<typename... Args>
            constexpr Iterator emplace(ConstIterator pos, Args&&... args) = delete;
            constexpr Iterator erase(ConstIterator pos) = delete;
            constexpr Iterator erase(ConstIterator first, ConstIterator last) = delete;
        
        private:
            FSDS_StableList m_list;
    };
}

#include "stable_list.inl"

#endif //STABLE_LIST_HPP_HEADER_GUARD