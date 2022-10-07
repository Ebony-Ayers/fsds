namespace fsds
{
	template<typename T>
	constexpr FastInsertList<T>::FastInsertList() noexcept
	: m_rawList(), m_nextAvailable(nullptr)
	{}
	template<typename T>
	constexpr FastInsertList<T>::FastInsertList(const size_t& count)
	: m_rawList(count), m_nextAvailable(nullptr)
	{}
	template<typename T>
	constexpr FastInsertList<T>::FastInsertList(const FastInsertList& other)
	: m_rawList(other.m_rawList), m_nextAvailable(other.m_nextAvailable)
	{}
	template<typename T>
	constexpr FastInsertList<T>::FastInsertList(FastInsertList&& other) noexcept
	: m_rawList(std::move(other.m_rawList)), m_nextAvailable(std::move(other.m_nextAvailable))
	{}
	template<typename T>
	constexpr FastInsertList<T>::~FastInsertList()
	{
		//
	}

	template<typename T>
	constexpr FastInsertList<T>& FastInsertList<T>::operator=(const FastInsertList& other)
	{
		this->m_rawList = other.m_rawList;
		this->m_nextAvailable = other.m_nextAvailable;
	}
	template<typename T>
	constexpr FastInsertList<T>& FastInsertList<T>::operator=(FastInsertList&& other) noexcept
	{
		this->m_rawList = std::move(other.m_rawList);
		this->m_nextAvailable = std::move(other.m_nextAvailable);
	}

	template<typename T>
	constexpr T& FastInsertList<T>::operator[](size_t pos)
	{
		return this->m_rawList[pos].data;
	}
	template<typename T>
	constexpr const T& FastInsertList<T>::operator[](size_t pos) const
	{
		return this->m_rawList[pos].data;
	}

	template<typename T>
	[[nodiscard]] constexpr bool FastInsertList<T>::isEmpty() const noexcept
	{
		return this->m_rawList.isEmpty();
	}
	template<typename T>
	constexpr size_t FastInsertList<T>::size() const noexcept
	{
		return this->m_rawList.size();
	}
	template<typename T>
	constexpr size_t FastInsertList<T>::capacity() const noexcept
	{
		return this->m_rawList.capacity();
	}

	template<typename T>
	constexpr void FastInsertList<T>::reserve(size_t newCap)
	{
		this->m_rawList.reserve(newCap);
	}
	template<typename T>
	constexpr void FastInsertList<T>::clear()
	{
		this->m_rawList.clear();
		this->m_nextAvailable = nullptr;
	}

	template<typename T>
	constexpr size_t FastInsertList<T>::add(const T& value)
	{
		//if there is no available slots in the existing list simply append the new data
		if(this->m_nextAvailable == nullptr)
		{
			this->m_rawList.append(static_cast<const FastInsertList<T>::ListElement>(value));
			return this->m_rawList.size() - 1;
		}
		//if there is space available take the frist available space and set the next one to be the currently available
		else
		{
			ListElement* location = this->m_nextAvailable;
			this->m_nextAvailable = this->m_nextAvailable->next.nextAvailable;
			location->data = value;
			return static_cast<size_t>(location - this->m_rawList.data());
		}
	}
	template<typename T>
	constexpr size_t FastInsertList<T>::add(T&& value)
	{
		//if there is no available slots in the existing list simply append the new data
		if(this->m_nextAvailable == nullptr)
		{
			this->m_rawList.append(static_cast<const FastInsertList<T>::ListElement>(value));
			return this->m_rawList.size() - 1;
		}
		//if there is space available take the frist available space and set the next one to be the currently available
		else
		{
			ListElement* location = this->m_nextAvailable;
			this->m_nextAvailable = this->m_nextAvailable->next.nextAvailable;
			location->data = value;
			return static_cast<size_t>(location - this->m_rawList.data());
		}
	}
	template<typename T>
	template<typename... Args>
	constexpr size_t FastInsertList<T>::addConstruct(Args&&... args)
	{
		if(this->m_nextAvailable == nullptr)
		{
			this->m_rawList.append(static_cast<const FastInsertList<T>::ListElement>(T(args...)));
			return this->m_rawList.size() - 1;
		}
		else
		{
			ListElement* location = this->m_nextAvailable;
			this->m_nextAvailable = this->m_nextAvailable.next.nextAvailable;
			location->data(args...);
			return static_cast<size_t>(location - this->m_rawList.data());
		}
	}

	template<typename T>
	constexpr void FastInsertList<T>::remove(size_t pos)
	{
		//removing data is quite concentually complex
		//terminology: valid data = data the has not been removed and accessing it value will result in valid data
		//terminology: available data = data that has been removed and accessing it will result in undefined behavior
		//terminoligy: element = the raw elemts of the list which is the struct contains the user data and the union
		//once data is removed it gets interpreted as a union. The first member of the union nextAvailable is a node of a linked list of all removed elemnts. The second member nextData of the union is the index of the next element that contains valid data.
		//terminology: chain = a series of elements of available data that are contiguous and thus all share a common nextData.
		//to explain the different possible cases I propose the following nomenclature
		//p = pos
		//* = a single element of valid data
		//** = one of more elements of valid data
		//*** = two of more elements of valid data
		//^ = an unknown number of elements of valid data
		//- = a single element of available data
		//--- = two or more elements of available data
		//# = any number of any combination of valid and available data
		//it is implied that there will be data at the start and end and thus *** can be excluded from the start and end
		//for example -*p => -*- implies that there is a element of available data then an element of valid data then the position. After pos being removed will have one element of available data then one element of valid data then one element of available data.
		//-p---*- => ---*- implies that there is a single element of available data followed by the pos being removed then two or more available data in a chain followed by one element of valid data then one element of available data. After removal there will be two or more available data followed by one element of valid data then one element of available data

		//the object will not be removed from memeory so if it is not trivially destructible it's destructor needs calling
		if constexpr(!std::is_trivially_destructible<T>::value)
		{
			this->m_rawList[pos].data.~T();
		}
		
		//if there are no available elements then setting the available element is trivial (p => -)
		if(this->m_nextAvailable == nullptr)
		{
			//std::cout << "case 1" << std::endl;
			
			this->m_rawList[pos].next.nextAvailable = nullptr;
			this->m_rawList[pos].next.nextData = pos + 1;
			this->m_nextAvailable = &(this->m_rawList[pos]);
		}
		//if there is only one available element
		else if(this->m_nextAvailable->next.nextAvailable == nullptr)
		{
			//std::cout << "case 2" << std::endl;
			
			//if pos is before the next available (p^- => -^-)
			if(static_cast<size_t>(this->m_nextAvailable - this->m_rawList.data()) > pos)
			{
				this->m_rawList[pos].next.nextAvailable = this->m_nextAvailable;
				//if the next available is right after pos form a chain otherwise don't (p- => ---)
				if(static_cast<size_t>(this->m_nextAvailable - this->m_rawList.data()) == pos + 1)
				{
					this->m_rawList[pos].next.nextData = this->m_nextAvailable->next.nextData;
				}
				//(p**- => -**-)
				else
				{
					this->m_rawList[pos].next.nextData = pos + 1;
				}
				this->m_nextAvailable = &(this->m_rawList[pos]);
			}
			//if pos is after next availble (-^p => -^-)
			else
			{
				this->m_rawList[pos].next.nextAvailable = nullptr;
				this->m_rawList[pos].next.nextData = pos + 1;
				this->m_nextAvailable->next.nextAvailable = &(this->m_rawList[pos]);
				//if next available is just before pos make a chain (-p => ---) otherwise next data won't change (-**p => -**-)
				if(static_cast<size_t>(this->m_nextAvailable - this->m_rawList.data()) + 1 == pos)
				{
					this->m_nextAvailable->next.nextData = this->m_rawList[pos].next.nextData;
				}
			}
		}
		else
		{
			//std::cout << "case 3" << std::endl;
			
			//search for where in the next available elements in the list is. they must be in order for trivial itterating so when removing the order must be maintained
			//the element pointing to element
			FastInsertList<T>::ListElement* previousElement = nullptr;
			//the first empty element who's next valid data is os ir after the desired position
			FastInsertList<T>::ListElement* element = this->m_nextAvailable;
			//if there are a series of continuous available elements which ends in element then this will be the pointer to the first
			FastInsertList<T>::ListElement* chainStart = nullptr;
			while(static_cast<size_t>(element->next.nextAvailable - this->m_rawList.data()) < pos)
			{
				//std::cout << "element loop iteration" << std::endl;
				previousElement = element;
				element = element->next.nextAvailable;

				//if previous element is right before element mark the start of the chain if it is not already marked
				if(static_cast<size_t>(previousElement - this->m_rawList.data()) + 1 == static_cast<size_t>(element - this->m_rawList.data()))
				{
					if(chainStart == nullptr)
					{
						chainStart = previousElement;
					}
				}
				//if previous element is not right before element reset the chain
				//there are two cases here one where the chainStart is already null and one where it isn't however given both cases end in null no check is required
				else
				{
					chainStart = nullptr;
				}
				
			}

			//to add to the previous nomenclature
			//e = element
			//ee = previous element
			//both e and ee are also -

			//if previous element is null then the element being removed is before nextAvailable or there is exactly one available data before pos
			if(previousElement == nullptr) [[unlikely]]
			{
				//std::cout << "case 4" << std::endl;

				//if pos is before element then pos is before nextAvailable as the smallest element can be is nextAvailable and pos is after nextAvailable then pos must be greater than element
				if(static_cast<size_t>(element - this->m_rawList.data()) > pos) // (p^e => -^e)
				{
					//std::cout << "case 5" << std::endl;
				
					this->m_rawList[pos].next.nextAvailable = element;
					//if nextAvailable is right after pos then form a chain
					if(static_cast<size_t>(this->m_nextAvailable - this->m_rawList.data()) == pos + 1) // (pe# => -e#)
					{
						//std::cout << "case 8" << std::endl;
				
						this->m_rawList[pos].next.nextData = element->next.nextData;
					}
					else // (p^e# => -^e#)
					{
						//std::cout << "case 9" << std::endl;
				
						this->m_rawList[pos].next.nextData = pos + 1;
					}
					this->m_nextAvailable = &(this->m_rawList[pos]);
				}
				//if pos is after element and there is no previous element
				//becuase the case of only one element removed has been handeled so far having elememnt before pos imples that element < pos < some available data
				else // (e^p# => e^-#)
				{
					//std::cout << "case 10" << std::endl;
				
					this->m_rawList[pos].next.nextAvailable = element->next.nextAvailable;
					//if the next element is right after pos then form a chain
					if(static_cast<size_t>(element->next.nextAvailable - this->m_rawList.data()) == pos + 1) // (e^p--- => e^---)
					{
						//std::cout << "case 11" << std::endl;
				
						this->m_rawList[pos].next.nextData = element->next.nextAvailable->next.nextData;
					}
					else // (e^p** => e^-)
					{
						//std::cout << "case 12" << std::endl;
				
						this->m_rawList[pos].next.nextData = pos + 1;
					}
					element->next.nextAvailable = &(this->m_rawList[pos]);
					//if element is right before pos form a chain otherwise element's nextData will stay the same
					if(static_cast<size_t>(element - this->m_rawList.data()) + 1 == pos)
					{
						//std::cout << "case 13" << std::endl;
				
						element->next.nextData = this->m_rawList[pos].next.nextData;
					}
				}
			}
			else // there is a previous element
			{
				//std::cout << "case 15" << std::endl;
				
				//if the element is before or just before the pos
				if(element->next.nextData <= pos) // (ee^e^p# => ee^e^-#)
				{
					//std::cout << "case 16" << std::endl;
				
					//inserting the current element into the linked list will be the same reguardless of any chains of elemnts
					//as element is no available data before element, element must point to data passed pos
					this->m_rawList[pos].next.nextAvailable = element->next.nextAvailable;
					
					//a contiguous chain of elements only have the nextData in common
					size_t nextValidData;
					//if there is an available element right after the one we are removing we need make a chain
					if(static_cast<size_t>(element->next.nextAvailable - this->m_rawList.data()) == pos + 1) // (ee^e^p-# => ee^e^---)
					{
						//std::cout << "case 17" << std::endl;
				
						//find the last element which is contiguous after pos
						FastInsertList<T>::ListElement* secondChainStart;
						while(static_cast<size_t>(secondChainStart->next.nextAvailable - this->m_rawList.data()) == static_cast<size_t>((secondChainStart - this->m_rawList.data()) + 1))
						{
							secondChainStart = secondChainStart->next.nextAvailable;
						}
						nextValidData = secondChainStart->next.nextData;
					}
					//if there is no available element after the one we are removing then we don't for a chain
					else // (ee^e^p => ee^e^-)
					{
						//std::cout << "case 18" << std::endl;
				
						nextValidData = pos + 1;
					}
					//now that we know the next valid data is we can finish setting the element we are removing
					this->m_rawList[pos].next.nextData = nextValidData;

					//if element is just before pos then there is a chain of available elements and we need to update the nextData for all of them.
					//The case of (eee**p#) is irellevant as it doesn't effect p
					if(static_cast<size_t>(element - this->m_rawList.data()) + 1 == pos) 
					{
						//std::cout << "case 19" << std::endl;
				
						//if previous element is just before element then previous element element and pos wil form a single chain
						//the case of (ee**ep# => ee**---) is irrelevent as it does not change anything
						if(static_cast<size_t>(previousElement - this->m_rawList.data()) + 1 == static_cast<size_t>(element - this->m_rawList.data())) // (eeep# => ---)
						{
							//std::cout << "case 20" << std::endl;
				
							while(chainStart < element)
							{
								chainStart->next.nextData = nextValidData;
								chainStart = chainStart->next.nextAvailable;
							}
						}

						element->next.nextData = nextValidData;
					}

					//the previous element can only be set at the end as it's old data is required beforehand
					element->next.nextAvailable = &(this->m_rawList[pos]);
				}
				//if element is not before pos then element is after pos. if previous element was after pos then it would be element. this implies previousElement < pos < element
				else // (ee^p^e# => ee^-^e#)
				{
					//std::cout << "case 23" << std::endl;
				
					this->m_rawList[pos].next.nextAvailable = element;
					//if element is right after pos, form a chain 
					if(element == &(this->m_rawList[pos+1])) // (ee^pe# => ee^---#)
					{
						//std::cout << "case 24" << std::endl;
				
						this->m_rawList[pos].next.nextData = element->next.nextData;
					}
					else // (ee^p**e# => ee^-**e#)
					{
						//std::cout << "case 25" << std::endl;
				
						this->m_rawList[pos].next.nextData = pos + 1;
					}
					//add pos to the linked list of available elements
					previousElement->next.nextAvailable = &(this->m_rawList[pos]);
					//if the previous element is right before pos then it's next data is the same as pos forming a chain
					//in the case of (eepe) becuase nextData of p has been set before nothing special needs to be done. Therefore all cases of (eep^e) are the same
					if(previousElement == &(this->m_rawList[pos-1])) // (eep^e)
					{
						//std::cout << "case 26" << std::endl;
				
						previousElement->next.nextData = this->m_rawList[pos].next.nextData;
					}
				}
			}
		}
	}

	template<typename T>
	constexpr bool FastInsertList<T>::valueEquality(const FastInsertList<T>& other) const
	{
		if(this->size() != other.size())
		{
			return false;
		}
		
		auto thisIt1 = this->getIterator();
		auto otherIt1 = other.getIterator();
		auto thisIt2 = thisIt1.getDiscontigousIterator(*this);
		auto otherIt2 = otherIt1.getDiscontigousIterator(other);

		while(thisIt1.notDone() && otherIt1.notDone())
		{
			if(thisIt1.getConst(*this) != otherIt1.getConst(other))
			{
				return false;
			}
			
			thisIt1.nextConst(*this);
			otherIt1.nextConst(other);
		}

		if(thisIt1.shouldGetDiscontigousIterator(*this) || otherIt1.shouldGetDiscontigousIterator(other))
		{
			if(thisIt1.notDone())
			{
				if(otherIt1.shouldGetDiscontigousIterator(other))
				{
					while(thisIt1.notDone() && otherIt2.notDone(other))
					{
						if(thisIt1.getConst(*this) != otherIt2.getConst(other))
						{
							return false;
						}
						
						thisIt1.nextConst(*this);
						otherIt2.nextConst(other);
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				if(thisIt1.shouldGetDiscontigousIterator(*this))
				{
					while(thisIt2.notDone(*this) && otherIt1.notDone())
					{
						if(thisIt2.getConst(*this) != otherIt1.getConst(other))
						{
							return false;
						}

						thisIt2.nextConst(*this);
						otherIt1.nextConst(other);
					}
				}
				else
				{
					return false;
				}
			}

			if(thisIt1.shouldGetDiscontigousIterator(*this) && otherIt1.shouldGetDiscontigousIterator(other))
			{
				while(thisIt2.notDone(*this) && otherIt2.notDone(other))
				{
					if(thisIt2.getConst(*this) != thisIt2.getConst(other))
					{
						return false;
					}

					thisIt2.nextConst(*this);
					otherIt2.nextConst(other);
				}
			}
		}

		return true;
	}

	template<typename T>
	constexpr void FastInsertList<T>::deepCopy(FastInsertList<T>& dest) const
	{
		this->m_rawList.deepCopy(dest.m_rawList);
		dest.m_nextAvailable = this->m_nextAvailable;
	}

	template<typename T>
	constexpr FastInsertListContiguousIterator<T> FastInsertList<T>::getIterator() const
	{
		return FastInsertListContiguousIterator<T>(*this);
	}


	template<typename T>
	constexpr FastInsertListDiscontiguousIterator<T>::FastInsertListDiscontiguousIterator(const FastInsertList<T>& l)
	: m_index(0), m_nextAvailable(nullptr)
	{
		if(l.m_nextAvailable != nullptr)
		{
			this->m_index = l.m_nextAvailable->next.nextData;
			this->m_nextAvailable = l.m_nextAvailable->next.nextAvailable;
		}

		while(static_cast<size_t>(this->m_nextAvailable - l.m_rawList.data()) <= m_index)
		{
			this->m_nextAvailable = this->m_nextAvailable->next.nextAvailable;
		}
	}
	template<typename T>
	constexpr T& FastInsertListDiscontiguousIterator<T>::get(FastInsertList<T>& l) const
	{
		return l[this->m_index];
	}
	template<typename T>
	constexpr const T& FastInsertListDiscontiguousIterator<T>::getConst(const FastInsertList<T>& l) const
	{
		return l[this->m_index];
	}
	template<typename T>
	constexpr T& FastInsertListDiscontiguousIterator<T>::next(FastInsertList<T>& l)
	{
		if(&(l.m_rawList[this->m_index+1]) == this->m_nextAvailable)
		{
			this->m_index = this->m_nextAvailable->next.nextData;
			this->m_nextAvailable = l.m_rawList[this->m_nextAvailable->next.nextData - 1].next.nextAvailable;
		}
		else
		{
			this->m_index++;
		}
		return l[this->m_index];
	}
	template<typename T>
	constexpr const T& FastInsertListDiscontiguousIterator<T>::nextConst(const FastInsertList<T>& l)
	{
		if(&(l.m_rawList[this->m_index+1]) == this->m_nextAvailable)
		{
			this->m_index = this->m_nextAvailable->next.nextData;
			this->m_nextAvailable = l.m_rawList[this->m_nextAvailable->next.nextData - 1].next.nextAvailable;
		}
		else
		{
			this->m_index++;
		}
		return l[this->m_index];
	}
	template<typename T>
	constexpr bool FastInsertListDiscontiguousIterator<T>::notDone(const FastInsertList<T>& l) const
	{
		return (this->m_index < l.size());
	}


	template<typename T>
	constexpr FastInsertListContiguousIterator<T>::FastInsertListContiguousIterator(const FastInsertList<T>& l)
	: m_index(0), m_finish()
	{
		if(l.m_nextAvailable == nullptr)
		{
			this->m_finish = l.size();
		}
		else
		{
			this->m_finish = static_cast<size_t>(l.m_nextAvailable - l.m_rawList.data());
		}
	}
	template<typename T>
	constexpr T& FastInsertListContiguousIterator<T>::get(FastInsertList<T>& l) const
	{
		return l[this->m_index];
	}
	template<typename T>
	constexpr const T& FastInsertListContiguousIterator<T>::getConst(const FastInsertList<T>& l) const
	{
		return l[this->m_index];
	}
	template<typename T>
	constexpr T& FastInsertListContiguousIterator<T>::next(FastInsertList<T>& l)
	{
		this->m_index++;
		return l[this->m_index];
	}
	template<typename T>
	constexpr const T& FastInsertListContiguousIterator<T>::nextConst(const FastInsertList<T>& l)
	{
		this->m_index++;
		return l[this->m_index];
	}
	template<typename T>
	constexpr bool FastInsertListContiguousIterator<T>::notDone() const
	{
		return (this->m_index < this->m_finish);
	}
	template<typename T>
	constexpr FastInsertListDiscontiguousIterator<T> FastInsertListContiguousIterator<T>::getDiscontigousIterator(const FastInsertList<T>& l) const
	{
		return FastInsertListDiscontiguousIterator(l);
	}

	template<typename T>
	constexpr bool FastInsertListContiguousIterator<T>::shouldGetDiscontigousIterator(const FastInsertList<T>& l) const
	{
		if(this->m_index >= l.size() - 1)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
};