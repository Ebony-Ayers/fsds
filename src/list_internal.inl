namespace fsds
{
	namespace listInternalFunctions
    {
        //you need to manually allocate space for data. capacity is how many elements were allocated
        template<typename T>
        constexpr void constructCount(ListHeader& header, T* const data, const size_t& capacity)
        {
            header.size = 0;
            header.capacity = capacity;
        }
        //you need to manually allocate space for data. capacity is how many elements were allocated
        template<typename T>
        constexpr void copyConstructor(ListHeader& thisHeader, T* const thisData, ListHeader& otherHeader, T* const otherData, const size_t& capacity)
        {
            thisHeader.size = otherHeader.size;
            thisHeader.capacity = capacity;
            std::copy(otherData, otherData + otherHeader.size, thisData);
        }
        template<typename T>
        //the move constructor does not require space to be allocated
        constexpr void moveConstructor(ListHeader& thisHeader, T* const thisData, ListHeader& otherHeader, T* const otherData)
        {
            thisHeader.size = otherHeader.size;
            thisHeader.capacity = otherHeader.capacity;
            thisData = otherData;
        }
        template<typename T>
        //you need to manually allocate space for data. capacity is how many elements were allocated
        constexpr void initialiserListConstructor(ListHeader& header, T* data, std::initializer_list<T> init, const size_t& capacity)
        {
            header.size = init.size();
            header.capacity = capacity;
            std::copy(init.begin(), init.end(), data);
        }
        template<typename T>
        constexpr void destructor(ListHeader& header, T* data)
        {
            fsds::listInternalFunctions::desconstructAllElements(header, data);
        }

        //you need to manually allocate space for data. capacity is how many elements were allocated
        template<typename T>
        constexpr void copyAssignmentOperator(ListHeader& thisHeader, T* const thisData, ListHeader& otherHeader, T* const otherData, const size_t& capacity)
        {
            thisHeader.size = otherHeader.size;
            thisHeader.capacity = capacity;
            std::copy(otherData, otherData + otherHeader.size, thisData);
        }
        //move assignment does not require reallocating space
        template<typename T>
        constexpr void moveAssignmentOperator(ListHeader& thisHeader, T* const thisData, ListHeader& otherHeader, T* const otherData)
        {
            thisHeader.size = otherHeader.size;
            thisHeader.capacity = otherHeader.capacity;
            thisData = otherData;
        }

        template<typename T>
        constexpr T& elementAccessOperator(ListHeader& header, T* const data, const size_t& pos)
        {
            if(header.size == 0) [[unlikely]]
			{
				throw std::out_of_range("fsds::listInternalFunctions::elementAccessOperator() attempting to access list with no data");
			}
            else if (pos >= header.size) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::elementAccessOperator() index out of range");
            }
            
            return data[pos];
        }
        template<typename T>
        constexpr const T& constElementAccessOperator(const ListHeader& header, const T* const data, const size_t& pos)
        {
            if(header.size == 0) [[unlikely]]
			{
				throw std::out_of_range("fsds::listInternalFunctions::constElementAccessOperator() attempting to access list with no data");
			}
            else if (pos >= header.size) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::constElementAccessOperator() index out of range");
            }
            
            return data[pos];
        }

        template<typename T>
        constexpr T& front(ListHeader& header, T* const data)
        {
            if(header.size == 0) [[unlikely]]
			{
				throw std::out_of_range("fsds::listInternalFunctions::front() attempting to access list with no data");
			}
            
            return data[0];
        }
        template<typename T>
        constexpr const T& constFront(const ListHeader& header, const T* const data)
        {
            if(header.size == 0) [[unlikely]]
			{
				throw std::out_of_range("fsds::listInternalFunctions::constFront() attempting to access list with no data");
			}
            
            return data[0];
        }
        template<typename T>
        constexpr T& back(ListHeader& header, T* const data)
        {
            if(header.size == 0) [[unlikely]]
			{
				throw std::out_of_range("fsds::listInternalFunctions::back() attempting to access list with no data");
			}
            
            return data[header.size-1];
        }
        template<typename T>
        constexpr const T& constBack(const ListHeader& header, const T* const data)
        {
            if(header.size == 0) [[unlikely]]
			{
				throw std::out_of_range("fsds::listInternalFunctions::constBack() attempting to access list with no data");
			}

            return data[header.size-1];
        }

        template<typename T>
        constexpr bool isEmpty(const ListHeader& header, const T* const /*data*/)
        {
            return header.size == 0;
        }
        template<typename T>
        constexpr size_t size(const ListHeader& header, const T* const /*data*/)
        {
            return header.size;
        }
        template<typename T>
        constexpr size_t maxSize(const ListHeader& header, const T* const /*data*/)
        {
            return std::numeric_limits<size_t>::max() / sizeof(T);
        }
        template<typename T>
        constexpr size_t capacity(const ListHeader& header, const T* const /*data*/)
        {
            return header.capacity;
        }

        template<typename T>
        constexpr void clear(ListHeader& header, T* const data)
        {
            fsds::listInternalFunctions::desconstructAllElements(header, data);
            header.size = 0;
        }

        //number of elements not bytes required when reallocating to allow the opperation
        //if the number is zero then no reallocation is required
        //if a reallocation is required it is assumed that no copying and no deallocating has taken place yet
        //must always be called before respective modifying function
        template<typename T>
        constexpr size_t appendReallocationMinimumElements(ListHeader& header, T* const data)
        {
            return header.size + 1;
        }
        template<typename T>
        constexpr size_t prependReallocationMinimumElements(ListHeader& header, T* const data)
        {
            return header.size + 1;
        }
        template<typename T>
        constexpr size_t insertReallocationMinimumElements(ListHeader& header, T* const data)
        {
            return header.size + 1;
        }
        //if no reallocation was needed newData should be the same as oldData
        //If a reallocation happened oldData should be the unmodified original data and newData is the pointer to the newlt allocated uninitialised memeory
        //returns a pointer to the old memory for deallocation. This pointer should only be deallocated if a reallocation previously took place
        template<typename T>
        constexpr T* append(ListHeader& header, T* const oldData, T* const newData, const T& value)
        {
            if(oldData != newData)
            {
                std::copy(oldData, oldData + header.size, newData);
            }
            newData[header.size] = value;
            header.size++;
            return oldData;
        }
        template<typename T>
        constexpr T* prepend(ListHeader& header, T* const oldData, T* const newData, const T& value)
        {
            std::copy_backward(oldData, oldData + header.size, newData + header.size + 1);
            newData[0] = value;
            header.size++;
            return oldData;
        }
        template<typename T>
        constexpr T* insert(ListHeader& header, T* const oldData, T* const newData, const size_t& pos, const T& value)
        {
            std::copy_backward(oldData + pos, oldData + header.size, newData + header.size + 1);
            if(oldData != newData)
            {
                std::copy(oldData, oldData + pos, newData);
            }
            newData[pos] = value;
            header.size++;
            return oldData;
        }
        template<typename T, typename... Args>
        constexpr T* appendConstruct(ListHeader& header, T* const oldData, T* const newData, Args&&... args)
        {
            if(oldData != newData)
            {
                std::copy(oldData, oldData + header.size, newData);
            }
            std::construct_at(newData + header.size, args...);
            header.size++;
            return oldData;
        }
        template<typename T, typename... Args>
        constexpr T* prependConstruct(ListHeader& header, T* const oldData, T* const newData, Args&&... args)
        {
            std::copy_backward(oldData, oldData + header.size, newData + header.size + 1);
            std::construct_at(newData, args...);
            header.size++;
            return oldData;
        }
        template<typename T, typename... Args>
        constexpr T* insertConstruct(ListHeader& header, T* const oldData, T* const newData, const size_t& pos, Args&&... args)
        {
            if (pos >= header.size) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::insertConstruct() index out of range");
            }

            std::copy_backward(oldData + pos, oldData + header.size, newData + header.size + 1);
            if(oldData != newData)
            {
                std::copy(oldData, oldData + pos, newData);
            }
            std::construct_at(newData + pos, args...);
            header.size++;
            return oldData;
        }

        template<typename T>
        constexpr void remove(ListHeader& header, T* const data, size_t pos)
        {
            if(header.size == 0) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::remove() attempting to remove from list with no data");
            }
            else if (pos >= header.size) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::remove() index out of range");
            }
            
            std::copy(data + pos + 1, data + header.size, data + pos);
            header.size--;
        }
        template<typename T>
        constexpr void removeBack(ListHeader& header, T* const /*data*/)
        {
            if(header.size == 0) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::removeBack() attempting to remove from list with no data");
            }
            
            header.size--;
        }
        template<typename T>
        constexpr void removeFront(ListHeader& header, T* const data)
        {
            if(header.size == 0) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::removeFront() attempting to remove from list with no data");
            }

            std::copy(data + 1, data + header.size, data);
            header.size--;
        }
        template<typename T>
        constexpr void removeDeconstruct(ListHeader& header, T* const data, size_t pos)
        {
            if(header.size == 0) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::removeDeconstruct() attempting to remove from list with no data");
            }
            else if (pos >= header.size) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::removeDeconstruct() index out of range");
            }
            
            std::destroy_at(data + pos);
            std::copy(data + pos + 1, data + header.size, data + pos);
            header.size--;
        }
        template<typename T>
        constexpr void removeBackDeconstruct(ListHeader& header, T* const data)
        {
            if(header.size == 0) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::removeBackDeconstruct() attempting to remove from list with no data");
            }

            std::destroy_at(data + header.size - 1);
            header.size--;
        }
        template<typename T>
        constexpr void removeFrontDeconstruct(ListHeader& header, T* const data)
        {
            if(header.size == 0) [[unlikely]]
            {
                throw std::out_of_range("fsds::listInternalFunctions::removeFrontDeconstruct() attempting to remove from list with no data");
            }

            std::destroy_at(data);
            std::copy(data + 1, data + header.size, data);
            header.size--;
        }

        template<typename T>
        constexpr bool valueEquality(const ListHeader& thisHeader, const T* const thisData, const ListHeader& otherHeader, const T* const otherData)
        {
            if(thisHeader.size != otherHeader.size)
            {
                return false;
            }
            else
            {
                if(thisData == otherData)
                {
                    return true;
                }
                else
                {
                    for(size_t i = 0; i < thisHeader.size; i++)
                    {
                        if(thisData[i] != otherData[i])
                        {
                            return false;
                        }
                    }
                    return true;
                }
            }
        }

        template<typename T>
        constexpr void deepCopy(const ListHeader& thisHeader, const T* const thisData, ListHeader& destHeader, T* const destData)
        {
            std::copy(thisData, thisData + thisHeader.size, destData);
            destHeader.size = thisHeader.size;
        }

        template<typename T>
        constexpr void desconstructAllElements(ListHeader& header, T* const data)
        {
            if(!std::is_trivially_destructible<T>::value)
            {
                for(size_t i = 0; i < header.size; i++)
                {
                    std::destroy_at(data + i);
                }
            }
        }

        template<typename AllocatorT, typename T>
        constexpr T* reallocationAllocateHelper(ListHeader& header, T* const data, const size_t& desiredCapacity)
        {
            const size_t desieredNumelements = desiredCapacity / sizeof(T);
            if(header.size >= header.capacity) [[unlikely]]
            {
                if(desieredNumelements > header.size) [[likely]]
                {
                    AllocatorT alloc;
                    T* newData = alloc.allocate(desieredNumelements);
                    header.capacity = desieredNumelements;
                    return newData;
                }
                else
                {
                    throw std::out_of_range("fsds::listInternalFunctions::reallocationHelper() attempting to realocate less space then is already allocated.");
                }
            }
            else
            {
                return data;
            }
        }
        template<typename AllocatorT, typename T>
        constexpr void reallocationDeallocateHelper(const ListHeader& /*header*/, T* const oldData, const T* const newData, const size_t& oldCapacity)
        {
            if(newData != oldData) [[unlikely]]
            {
                AllocatorT alloc;
                alloc.deallocate(oldData, oldCapacity);
            }
        }
    }
}