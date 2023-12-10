#ifndef LIST_INTERNAL_HEADER_GUARD
#define LIST_INTERNAL_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include <iostream>

namespace fsds
{
    namespace listInternalFunctions
    {
        struct ListHeader
        {
            size_t size;
			size_t capacity;
            size_t front;
        };

        //you need to manually allocate space for data. capacity is how many elements were allocated
        template<typename T>
        constexpr void constructCount(ListHeader& header, T* const data, const size_t& capacity);
        //you need to manually allocate space for data. capacity is how many elements were allocated
        template<typename T>
        constexpr void copyConstructor(ListHeader& thisHeader, T* const thisData, ListHeader& otherHeader, T* const otherData, const size_t& capacity);
        template<typename T>
        //the move constructor does not require space to be allocated
        constexpr void moveConstructor(ListHeader& thisHeader, T* const thisData, ListHeader& otherHeader, T* const otherData, const size_t& front);
        template<typename T>
        //you need to manually allocate space for data. capacity is how many elements were allocated
        constexpr void initialiserListConstructor(ListHeader& header, T* data, std::initializer_list<T> init, const size_t& capacity);
        template<typename T>
        constexpr void destructor(ListHeader& header, T* data);

        //you need to manually allocate space for data. capacity is how many elements were allocated
        template<typename T>
        constexpr void copyAssignmentOperator(ListHeader& thisHeader, T* const thisData, ListHeader& otherHeader, T* const otherData, const size_t& capacity);
        //move assignment does not require reallocating space
        template<typename T>
        constexpr void moveAssignmentOperator(ListHeader& thisHeader, T* const thisData, ListHeader& otherHeader, T* const otherData);

        template<typename T>
        constexpr T& elementAccessOperator(ListHeader& header, T* const data, const size_t& pos);
        template<typename T>
        constexpr const T& constElementAccessOperator(const ListHeader& header, const T* const data, const size_t& pos);

        template<typename T>
        constexpr T& front(ListHeader& header, T* const data);
        template<typename T>
        constexpr const T& constFront(const ListHeader& header, const T* const data);
        template<typename T>
        constexpr T& back(ListHeader& header, T* const data);
        template<typename T>
        constexpr const T& constBack(const ListHeader& header, const T* const data);

        template<typename T>
        constexpr bool isEmpty(const ListHeader& header, const T* const data);
        template<typename T>
        constexpr size_t size(const ListHeader& header, const T* const data);
        template<typename T>
        constexpr size_t maxSize(const ListHeader& header, const T* const data);
        template<typename T>
        constexpr size_t capacity(const ListHeader& header, const T* const data);

        template<typename T>
        constexpr void clear(ListHeader& header, T* const data);

        //number of elements not bytes required when reallocating to allow the opperation
        //if the number is zero then no reallocation is required
        //if a reallocation is required it is assumed that no copying and no deallocating has taken place yet
        //must always be called before respective modifying function
        template<typename T>
        constexpr size_t appendReallocationMinimumElements(ListHeader& header, T* const data);
        template<typename T>
        constexpr size_t prependReallocationMinimumElements(ListHeader& header, T* const data);
        template<typename T>
        constexpr size_t insertReallocationMinimumElements(ListHeader& header, T* const data);
        //if no reallocation was needed newData should be the same as oldData
        //If a reallocation happened oldData should be the unmodified original data and newData is the pointer to the newlt allocated uninitialised memeory. In addition the capacity of the header must be updated externally as the append functions do not modify it.
        //returns a pointer to the old memory for deallocation. This pointer should only be deallocated if a reallocation previously took place
        template<typename T>
        constexpr T* append(ListHeader& header, T* const oldData, T* const newData, const T& value);
        template<typename T>
        constexpr T* prepend(ListHeader& header, T* const oldData, T* const newData, const T& value);
        template<typename T>
        constexpr T* insert(ListHeader& header, T* const oldData, T* const newData, const size_t& pos, const T& value);
        template<typename T, typename... Args>
        constexpr T* appendConstruct(ListHeader& header, T* const oldData, T* const newData, Args&&... args);
        template<typename T, typename... Args>
        constexpr T* prependConstruct(ListHeader& header, T* const oldData, T* const newData, Args&&... args);
        template<typename T, typename... Args>
        constexpr T* insertConstruct(ListHeader& header, T* const oldData, T* const newData, const size_t& pos, Args&&... args);

        template<typename T>
        constexpr void remove(ListHeader& header, T* const data, size_t pos);
        template<typename T>
        constexpr void removeBack(ListHeader& header, T* const data);
        template<typename T>
        constexpr void removeFront(ListHeader& header, T* const data);
        template<typename T>
        constexpr void removeDeconstruct(ListHeader& header, T* const data, size_t pos);
        template<typename T>
        constexpr void removeBackDeconstruct(ListHeader& header, T* const data);
        template<typename T>
        constexpr void removeFrontDeconstruct(ListHeader& header, T* const data);

        template<typename T>
        constexpr bool valueEquality(const ListHeader& thisHeader, const T* const thisData, const ListHeader& otherHeader, const T* const otherData);

        template<typename T>
        constexpr void deepCopy(const ListHeader& thisHeader, const T* const thisData, ListHeader& destHeader, T* const destData);

        template<typename T>
        constexpr void desconstructAllElements(ListHeader& header, T* const data);

        template<typename AllocatorT, typename T>
        constexpr T* reallocationAllocateHelper(ListHeader& header, T* const data, const size_t& desiredCapacity);
        template<typename AllocatorT, typename T>
        constexpr void reallocationDeallocateHelper(const ListHeader& header, T* const oldData, const T* const newData, const size_t& oldCapacity);
    }
}

#include "list_internal.inl"

#endif //#ifndef LIST_INTERNAL_HEADER_GUARD