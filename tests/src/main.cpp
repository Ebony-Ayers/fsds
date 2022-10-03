#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <vector>
#include <source_location>
#include <algorithm>

#define FSDS_DEBUG
#include "../../src/fsds.hpp"

/*
//#define DEBUG_ALLOCATE_LOG
template<typename T>
class DebugAllocator
{
	public:
		constexpr DebugAllocator() noexcept
		: m_alloc(std::allocator<T>())
		{}

		[[nodiscard]] constexpr T* allocate(size_t n, const std::source_location location = std::source_location::current())
		{
			T* p = m_alloc.allocate(n);
			#ifdef DEBUG_ALLOCATE_LOG
				std::cout << "** allocated\t p=0x" << std::hex << reinterpret_cast<uint64_t>(p) << std::dec << "\tn=" << n;
				std::cout << "\t" << location.file_name() << ":" << location.line() << ":" << location.column() << ": " << location.function_name() << std::endl;
			#endif
			return p;
		}
		constexpr void deallocate(T* p, size_t n, const std::source_location location = std::source_location::current())
		{
			#ifdef DEBUG_ALLOCATE_LOG
				std::cout << "** deallocated\t p=0x" << std::hex << reinterpret_cast<uint64_t>(p) << std::dec << "\tn=" << n;
				std::cout << "\t" << location.file_name() << ":" << location.line() << ":" << location.column() << ": " << location.function_name() << std::endl;
			#endif
			m_alloc.deallocate(p, n);
		}
	
	private:
		std::allocator<T> m_alloc;
};
*/

void testFinitePQueue()
{
	std::vector<size_t> testsFailed;

	size_t testSize = 100;
	size_t numPriorities = 6;
	fsds::FinitePQueue<size_t, 6> pqueue;

	//test 1: simple enqueue and dequeue
	for(size_t i = 0; i < testSize; i++)
	{
		pqueue.enqueue(i, 5);
	}
	for(size_t i = 0; i < testSize; i++)
	{
		auto result = pqueue.dequeue(5);
		if(result != i)
		{
			testsFailed.push_back(1);
			break;
		}
	}

	
	//test 2: check that after enqueuing and dequeuing the same number of elements from an empty queue the size of the queue is 0
	if(pqueue.size() != 0)
	{
		testsFailed.push_back(2);
	}

	//test 3: check enough blocks have been allocated
	if(pqueue.capacity() != 144)
	{
		testsFailed.push_back(3);
	}

	//test 4: check the queue has been reset
	pqueue.clear();
	if((pqueue.capacity() != 48) && (pqueue.size() == 0))
	{
		testsFailed.push_back(4);
	}
	
	//test 5: enqueue and dequeue on all priorities
	for(size_t i = 0; i < testSize; i++)
	{
		//std::cout << ">> " << i << std::endl;
		pqueue.enqueue(i, i % numPriorities);
	}
	for(size_t i = 0; i < testSize; i++)
	{
		auto result = pqueue.dequeue(i % numPriorities);
		if(result != i)
		{
			testsFailed.push_back(5);
			break;
		}
	}

	//test 6: tryDequeue()
	pqueue.enqueue(100, 0);
	{
		size_t dest;
		if(pqueue.tryDequeue(&dest, 1) != false)
		{
			testsFailed.push_back(6);
		}
		if(pqueue.tryDequeue(&dest, 0) != true)
		{
			testsFailed.push_back(6);
		}
		if(dest != 100)
		{
			testsFailed.push_back(6);
		}
	}

	//test 7: front()
	pqueue.enqueue(100, 0);
	if(pqueue.front(0) != 100)
	{
		testsFailed.push_back(7);
	}

	//test 8: isEmpty()
	if(pqueue.isEmpty() != false)
	{
		testsFailed.push_back(8);
	}
	pqueue.dequeue(0);
	if(pqueue.isEmpty() != false)
	{
		testsFailed.push_back(8);
	}
	
	if(testsFailed.size() == 0)
	{
		std::cout << "FinitePQueue passed all tests" << std::endl;
	}
	else
	{
		if(testsFailed.size() == 1)
		{
			std::cout << "FinitePQueue failed test " << testsFailed[0] << std::endl;
		}
		else
		{
			std::cout << "FinitePQueue failed tests ";
			for(size_t i = 0; i < testsFailed.size() - 1; i++)
			{
				std::cout << testsFailed[i] << ", ";
			}
			std::cout << "and " << testsFailed[testsFailed.size()-1] << std::endl;
		}
	}
	
}

void testSPSCQueue()
{
	std::vector<size_t> testsFailed;

	size_t testSize = 100;
	fsds::SPSCQueue<size_t> queue;

	//test 1: simple encleue dequeue
	for(size_t i = 0; i < testSize; i++)
	{
		queue.enqueue(i);
	}
	for(size_t i = 0; i < testSize; i++)
	{
		auto val = queue.dequeue();
		if(val != i)
		{
			testsFailed.push_back(1);
			break;
		}
	}

	//tests 2: check isEmpty() Size() and Capacity() are correct for an empty queue
	if((queue.isEmpty() != true) || (queue.size() != 0) || (queue.capacity() != 128))
	{
		testsFailed.push_back(2);
	}

	//test 3: stagered enqueue and dequeue
	queue.enqueue(0);
	queue.enqueue(1);
	for(size_t i = 0; i < testSize; i++)
	{
		for(size_t j = 0; j < 4; j++)
		{
			queue.enqueue((i*4)+j+2);
		}
		for(size_t j = 0; j < 4; j++)
		{
			auto val = queue.dequeue();
			//manually handle val=1 and val=2 becuase of integer overflow
			if(val > 1)
			{
				if(val != (i*4)+j)
				{
					testsFailed.push_back(3);
					break;
				}
			}
		}
	}

	//test 4: check isEmpty() Size() and Capacity() are correct for a non empty queue
	if((queue.isEmpty() != false) || (queue.size() != 2) || (queue.capacity() != 128))
	{
		testsFailed.push_back(4);
	}

	//test 5: operator[]
	if((queue[0] != 400) || (queue[1] != 401))
	{
		testsFailed.push_back(5);
	}

	//test 6: front()
	if(queue.front() != queue[0])
	{
		testsFailed.push_back(6);
	}

	//test 7: reserve()
	queue.reserve(200);
	if((queue[0] != 400) || (queue[1] != 401) || (queue.capacity() != 200))
	{
		testsFailed.push_back(7);
		std::cout << queue.capacity() << std::endl;
	}
	queue.reserve(7);
	if((queue[0] != 400) || (queue[1] != 401) || (queue.capacity() != 7))
	{
		testsFailed.push_back(7);
	}

	//test 8: clear()
	queue.clear();
	if((queue.capacity() != 16) || (queue.size() != 0) || (queue.isEmpty() == false))
	{
		testsFailed.push_back(8);
	}

	//test 9: SPSCQueue(size_t)
	fsds::SPSCQueue<size_t> queue2(24);
	if((queue2.capacity() != 24) || (queue2.size() != 0))
	{
		testsFailed.push_back(9);
	}

	//test 10: operator==
	for(size_t i = 0; i < testSize; i++)
	{
		queue.enqueue(i);
		queue2.enqueue(i);
	}
	if(queue != queue2)
	{
		testsFailed.push_back(10);
	}

	//test 11: deepCopy
	queue.deepCopy(queue2);
	if(queue != queue2)
	{
		testsFailed.push_back(11);
	}

	//test 12: tryDequeue
	for(size_t i = 0; i < testSize; i++)
	{
		size_t res;
		bool success = queue.tryDequeue(&res);
		if((res != i) || (success != true))
		{
			testsFailed.push_back(12);
			break;
		}
	}
	{
		size_t res;
		if(queue.tryDequeue(&res) != false)
		{
			testsFailed.push_back(12);
		}
	}

	//test 13: operator=
	queue = queue2;
	if(queue != queue2)
	{
		testsFailed.push_back(13);
	}

	//test 14: copy constructor
	fsds::SPSCQueue<size_t> queue3(queue2);
	if(queue3 != queue2)
	{
		testsFailed.push_back(14);
	}

	//test 15: move operator=
	queue = std::move(queue3);
	if(queue != queue2)
	{
		testsFailed.push_back(15);
	}

	//test 16: move constructor
	fsds::SPSCQueue<size_t> queue4(std::move(queue2));
	if(queue4 != queue)
	{
		testsFailed.push_back(16);
	}

	if(testsFailed.size() == 0)
	{
		std::cout << "SPSCQueue passed all tests" << std::endl;
	}
	else
	{
		if(testsFailed.size() == 1)
		{
			std::cout << "SPSCQueue failed test " << testsFailed[0] << std::endl;
		}
		else
		{
			std::cout << "SPSCQueue failed tests ";
			for(size_t i = 0; i < testsFailed.size() - 1; i++)
			{
				std::cout << testsFailed[i] << ", ";
			}
			std::cout << "and " << testsFailed[testsFailed.size()-1] << std::endl;
		}
	}
}

void testList()
{
	std::vector<size_t> testsFailed;

	size_t testSize = 100;
	fsds::List<size_t> list;

	//test 1: append()
	for(size_t i = 0; i < testSize; i++)
	{
		list.append(i);
	}
	for(size_t i = 0; i < testSize; i++)
	{
		if(list.data()[i] != i)
		{
			testsFailed.push_back(1);
			break;
		}
	}

	//test 2: front() and back()
	if((list.front() != 0) || (list.back() != 99))
	{
		testsFailed.push_back(2);
	}

	//test 3: operator[]
	for(size_t i = 0; i < testSize; i++)
	{
		if(list[i] != i)
		{
			testsFailed.push_back(3);
			break;
		}
	}

	//test 4: size(), capacity(), and isEmpty()
	if((list.size() != 100) || (list.capacity() != 128) || (list.isEmpty() == true))
	{
		testsFailed.push_back(4);
	}

	//test 5: clear()
	list.clear();
	if((list.size() != 0) || (list.capacity() != 16) || (list.isEmpty() == false))
	{
		testsFailed.push_back(5);
	}

	//test 6: reserve()
	list.reserve(32);
	if(list.capacity() != 32)
	{
		testsFailed.push_back(6);
	}

	//test 7: prepend()
	for(size_t i = 0; i < testSize; i++)
	{
		list.prepend(i);
	}
	for(size_t i = 0; i < testSize; i++)
	{
		if(list[i] != testSize - i - 1)
		{
			testsFailed.push_back(7);
			break;
		}
	}

	//test 8: insert()
	for(size_t i = 0; i < testSize; i++)
	{
		list.insert(testSize/2, i);
	}
	for(size_t i = 0; i < testSize / 2; i++)
	{
		if(list[i] != testSize - i - 1)
		{
			testsFailed.push_back(8);
			break;
		}
	}
	for(size_t i = 0; i < testSize; i++)
	{
		if(list[i + (testSize / 2)] != testSize - i - 1)
		{
			testsFailed.push_back(8);
			break;
		}
	}
	for(size_t i = 0; i < testSize / 2; i++)
	{
		if(list[i + testSize + (testSize / 2)] != (testSize / 2) - i - 1)
		{
			testsFailed.push_back(8);
			break;
		}
	}

	//test 9: insert() at back and front
	list.insert(0, 999);
	list.insert(list.size(), 999);
	if((list[0] != 999) || (list[list.size()-1] != 999))
	{
		testsFailed.push_back(9);
	}

	//test 10: removeFront() and removeBack()
	list.removeFront();
	list.removeBack();
	if((list[0] != testSize-1) || (list[list.size()-1] != 0))
	{
		testsFailed.push_back(10);
	}

	//test 11: remove()
	for(size_t i = 0; i < testSize; i++)
	{
		list.remove(testSize/2);
	}
	for(size_t i = 0; i < testSize; i++)
	{
		if(list[i] != testSize - i - 1)
		{
			testsFailed.push_back(11);
			break;
		}
	}

	//test 12: appendConstruct() prependConstruct() and insert construct()
	class placeHolderClass
	{
		public:
			placeHolderClass(size_t x)
			{
				this->m_x = x;
			}

			size_t m_x;
	};
	fsds::List<placeHolderClass> list2;
	list2.appendConstruct(static_cast<size_t>(0));
	if(list2[0].m_x != 0)
	{
		testsFailed.push_back(12);
	}
	list2.prependConstruct(static_cast<size_t>(1));
	if(list2[0].m_x != 1)
	{
		testsFailed.push_back(12);
	}
	list2.insertConstruct(1, static_cast<size_t>(2));
	if(list2[1].m_x != 2)
	{
		testsFailed.push_back(3);
	}

	//test 13: operator==
	fsds::List<size_t> list3;
	fsds::List<size_t> list4;
	for(size_t i = 0; i < testSize; i++)
	{
		list3.append(i);
		list4.append(i);
	}
	if(!(list3 == list4))
	{
		testsFailed.push_back(13);
	}

	//test 14: operator=
	list = list4;
	if(!(list == list3))
	{
		testsFailed.push_back(14);
	}

	//test 15: copy constructor
	fsds::List<size_t> list5 = list;
	if(list5 != list)
	{
		testsFailed.push_back(15);
	}

	//test 16: move operator=
	list5 = std::move(list3);
	if((list5 != list4) || (list3.data() != nullptr))
	{
		testsFailed.push_back(16);
	}

	//test 17 move constructor
	fsds::List<size_t> list6 = std::move(list5);
	if((list6 != list4) || (list5.data() != nullptr))
	{
		testsFailed.push_back(17);
	}

	if(testsFailed.size() == 0)
	{
		std::cout << "List passed all tests" << std::endl;
	}
	else
	{
		if(testsFailed.size() == 1)
		{
			std::cout << "List failed test " << testsFailed[0] << std::endl;
		}
		else
		{
			std::cout << "List failed tests ";
			for(size_t i = 0; i < testsFailed.size() - 1; i++)
			{
				std::cout << testsFailed[i] << ", ";
			}
			std::cout << "and " << testsFailed[testsFailed.size()-1] << std::endl;
		}
	}
}

void fastInsertListRunSubTest(fsds::FastInsertList<size_t>& l, std::vector<size_t> initiallyRemoved, const size_t& toRemove, std::vector<size_t>& testsFailed, const size_t& testNum)
{
	//remove all the initially removed elements
	for(size_t i = 0; i < initiallyRemoved.size(); i++)
	{
		l.remove(initiallyRemoved[i]);
	}
	//remove the lement
	l.remove(toRemove);

	//add the element to be removed to initially removed to get the list of all removed elements then sort it to get the list in order of how they should be returned the add method
	initiallyRemoved.push_back(toRemove);
	std::sort(initiallyRemoved.begin(), initiallyRemoved.end());

	//add back all the removed elements and check they are in order
	//it is important to add each element back even after an error has been detected to return the fList to the original state it was in. If this isn't done the test program may hang on subsequence subtests.
	bool hasFailed = false;
	for(size_t i = 0; i < initiallyRemoved.size(); i++)
	{
		size_t location = l.add(i);
		if(location != initiallyRemoved[i])
		{
			hasFailed = true;
		}
	}
	if(hasFailed)
	{
		testsFailed.push_back(testNum);
		return;
	}

	//check that the data we added is what is expected
	for(size_t i = 0; i < initiallyRemoved.size(); i++)
	{
		if(l[initiallyRemoved[i]] != i)
		{
			testsFailed.push_back(testNum);
			return;
		}
	}
}

void testFastInsertList()
{
	std::vector<size_t> testsFailed;

	size_t testSize = 100;
	fsds::FastInsertList<size_t> fList;

	//test 1: add
	for(size_t i = 0; i < testSize; i++)
	{
		fList.add(i);
	}

	for(size_t i = 0; i < testSize; i++)
	{
		if(fList[i] != i)
		{
			testsFailed.push_back(i);
			break;
		}
	}

	//test 2: remove then add a single element
	size_t location;
	fList.remove(1);
	location = fList.add(10);
	if((fList[1] != 10) || (location != 1))
	{
		testsFailed.push_back(2);
	}

	//test 3: removing then adding elements contiguously in the order a,b
	fList.remove(1);
	fList.remove(2);
	location = fList.add(11);
	if((location != 1) || (fList[1] != 11))
	{
		testsFailed.push_back(3);
		fList.add(0);
	}
	else
	{
		location = fList.add(12);
		if((location != 2) || (fList[2] != 12))
		{
			testsFailed.push_back(3);
		}
	}

	//test 4: removing then adding elements contiguously in the order b,a
	fList.remove(2);
	fList.remove(1);
	location = fList.add(11);
	if((location != 1) || (fList[1] != 11))
	{
		testsFailed.push_back(4);
		fList.add(0);
	}
	else
	{
		location = fList.add(12);
		if((location != 2) || (fList[2] != 12))
		{
			testsFailed.push_back(4);
		}
	}

	//test 5: removing then adding elements with a gap in the order a,b
	fList.remove(1);
	fList.remove(3);
	location = fList.add(13);
	if((location != 1) || (fList[1] != 13))
	{
		testsFailed.push_back(5);
		fList.add(0);
	}
	else
	{
		location = fList.add(13);
		if((location != 3) || (fList[3] != 13))
		{
			testsFailed.push_back(5);
		}
	}

	//test 6: removing then adding elements with a gap in the order b,a
	fList.remove(3);
	fList.remove(1);
	location = fList.add(14);
	if((location != 1) || (fList[1] != 14))
	{
		testsFailed.push_back(6);
		fList.add(0);
	}
	else
	{
		location = fList.add(14);
		if((location != 3) || (fList[3] != 14))
		{
			testsFailed.push_back(6);
		}
	}

	
	//see src/fast_insert_list.inl fsds::FastInsertList<T>::remove for notation
	std::vector<size_t> initiallyRemoved;

	//(---p)
	initiallyRemoved = {1, 2};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 7);
	//(p---)
	initiallyRemoved = {2, 3};
	fastInsertListRunSubTest(fList, initiallyRemoved, 1, testsFailed, 8);
	//(---*p)
	initiallyRemoved = {1, 2};
	fastInsertListRunSubTest(fList, initiallyRemoved, 4, testsFailed, 9);
	//(p*---)
	initiallyRemoved = {3, 4};
	fastInsertListRunSubTest(fList, initiallyRemoved, 1, testsFailed, 10);
	//(-*-p)
	initiallyRemoved = {1, 3};
	fastInsertListRunSubTest(fList, initiallyRemoved, 4, testsFailed, 11);
	//(-*-*p)
	initiallyRemoved = {1, 3};
	fastInsertListRunSubTest(fList, initiallyRemoved, 5, testsFailed, 12);
	//(p-*-)
	initiallyRemoved = {2, 4};
	fastInsertListRunSubTest(fList, initiallyRemoved, 1, testsFailed, 13);
	//(p*-*-)
	initiallyRemoved = {3, 5};
	fastInsertListRunSubTest(fList, initiallyRemoved, 1, testsFailed, 14);
	//(-p-)
	initiallyRemoved = {1, 3};
	fastInsertListRunSubTest(fList, initiallyRemoved, 2, testsFailed, 15);
	//(-*p*-)
	initiallyRemoved = {1, 5};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 16);
	//(-*p-)
	initiallyRemoved = {1, 4};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 17);
	//(-p*-)
	initiallyRemoved = {1, 4};
	fastInsertListRunSubTest(fList, initiallyRemoved, 2, testsFailed, 18);
	

	//(---*-p)
	initiallyRemoved = {1, 2, 4};
	fastInsertListRunSubTest(fList, initiallyRemoved, 5, testsFailed, 19);
	//(---*p-)
	initiallyRemoved = {1, 2, 5};
	fastInsertListRunSubTest(fList, initiallyRemoved, 4, testsFailed, 20);
	
	//(-p*---)
	initiallyRemoved = {1, 4, 5};
	fastInsertListRunSubTest(fList, initiallyRemoved, 2, testsFailed, 21);
	//(p-*---)
	initiallyRemoved = {2, 4, 5};
	fastInsertListRunSubTest(fList, initiallyRemoved, 1, testsFailed, 22);
	//(---*---p)
	initiallyRemoved = {1, 2, 4, 5};
	fastInsertListRunSubTest(fList, initiallyRemoved, 6, testsFailed, 23);
	//(---*p---)
	initiallyRemoved = {1, 2, 5, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 4, testsFailed, 24);
	//(---p*---)
	initiallyRemoved = {1, 2, 5, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 25);
	//(p---*---)
	initiallyRemoved = {2, 3, 5, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 1, testsFailed, 26);
	//(---p---)
	initiallyRemoved = {1, 2, 4, 5};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 27);
	//(-p---)
	initiallyRemoved = {1, 3, 4};
	fastInsertListRunSubTest(fList, initiallyRemoved, 2, testsFailed, 28);
	//(---p-)
	initiallyRemoved = {1, 2, 4};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 29);
	//(---*p---)
	initiallyRemoved = {1, 2, 5, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 4, testsFailed, 30);
	//(---p*---)
	initiallyRemoved = {1, 2, 5, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 31);
	//(-*p---)
	initiallyRemoved = {1, 4, 5};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 32);
	//(---p*-)
	initiallyRemoved = {1, 2, 5};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 33);
	//(---*p*---)
	initiallyRemoved = {1, 2, 6, 7};
	fastInsertListRunSubTest(fList, initiallyRemoved, 5, testsFailed, 34);
	//(-*p*---)
	initiallyRemoved = {1, 5, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 35);
	//(---*p*-)
	initiallyRemoved = {1, 2, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 4, testsFailed, 36);
	//(--*--p--)
	initiallyRemoved = {1, 2, 4, 5, 7, 8};
	fastInsertListRunSubTest(fList, initiallyRemoved, 6, testsFailed, 37);
	//(--*-p--)
	initiallyRemoved = {1, 2, 4, 6, 7};
	fastInsertListRunSubTest(fList, initiallyRemoved, 5, testsFailed, 38);
	//(--*--p-)
	initiallyRemoved = {1, 2, 4, 5, 7};
	fastInsertListRunSubTest(fList, initiallyRemoved, 6, testsFailed, 39);
	//(-*--p--)
	initiallyRemoved = {1, 3, 4, 6, 7};
	fastInsertListRunSubTest(fList, initiallyRemoved, 5, testsFailed, 40);
	//(-*-p--)
	initiallyRemoved = {1, 3, 5, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 4, testsFailed, 41);
	//(-*--p-)
	initiallyRemoved = {1, 3, 4, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 5, testsFailed, 42);
	//(--p--*--)
	initiallyRemoved = {1, 2, 4, 5, 7, 8};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 43);
	//(--p-*--)
	initiallyRemoved = {1, 2, 4, 6, 7};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 44);
	//(--p--*-)
	initiallyRemoved = {1, 2, 4, 5, 7};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 45);
	//(-p--*--)
	initiallyRemoved = {1, 3, 4, 6, 7};
	fastInsertListRunSubTest(fList, initiallyRemoved, 2, testsFailed, 46);
	//(-p-*--)
	initiallyRemoved = {1, 3, 5, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 2, testsFailed, 47);
	//(-p--*-)
	initiallyRemoved = {1, 3, 4, 6};
	fastInsertListRunSubTest(fList, initiallyRemoved, 2, testsFailed, 48);
	//(--*--*--p)
	initiallyRemoved = {1, 2, 4, 5, 7, 8};
	fastInsertListRunSubTest(fList, initiallyRemoved, 9, testsFailed, 49);
	//(--*--*--*p)
	initiallyRemoved = {1, 2, 4, 5, 7, 8};
	fastInsertListRunSubTest(fList, initiallyRemoved, 10, testsFailed, 50);
	//(p--*--*--)
	initiallyRemoved = {2, 3, 5, 6, 8, 9};
	fastInsertListRunSubTest(fList, initiallyRemoved, 1, testsFailed, 51);
	//(p*--*--*--)
	initiallyRemoved = {3, 4, 6, 7, 9, 10};
	fastInsertListRunSubTest(fList, initiallyRemoved, 1, testsFailed, 52);
	//(--*--*--p--)
	initiallyRemoved = {1, 2, 4, 5, 7, 8, 10, 11};
	fastInsertListRunSubTest(fList, initiallyRemoved, 9, testsFailed, 53);
	//(--*--p--*--)
	initiallyRemoved = {1, 2, 4, 5, 7, 8, 10, 11};
	fastInsertListRunSubTest(fList, initiallyRemoved, 6, testsFailed, 53);
	//(--p--*--*--)
	initiallyRemoved = {1, 2, 4, 5, 7, 8, 10, 11};
	fastInsertListRunSubTest(fList, initiallyRemoved, 3, testsFailed, 53);

	//test 54: overwriting data
	for(size_t i = 0; i < testSize; i++)
	{
		fList[i] = i + 200;
	}
	for(size_t i = 0; i < testSize; i++)
	{
		if(fList[i] != i + 200)
		{
			testsFailed.push_back(54);
			break;
		}
	}

	//test 55, 56 and 57: iterator with removed data
	{
		std::vector<size_t> removed = {5, 6, 9, 11, 13, 14, 18, 19, 20, 22, 24, 26, 28, 30, 33, 36, 39, 42, 43, 44, 46, 47, 48, 50, 52, 54};
		for(size_t i = 0; i < removed.size(); i++)
		{
			fList.remove(removed[i]);
		}

		//test 55
		auto it1 = fList.getIterator();
		size_t i = 0;
		for(;it1.notDone(); it1.next(fList))
		{
			if(it1.get(fList) != i + 200)
			{
				testsFailed.push_back(55);
				break;
			}
			i++;
		}

		//test 56
		if(it1.shouldGetDiscontigousIterator(fList) != true)
		{
			testsFailed.push_back(56);
		}

		//test 57
		auto it2 = it1.getDiscontigousIterator(fList);
		i = 7;
		for(; it2.notDone(fList); it2.next(fList))
		{
			//if i in removed skip it
			while(true)
			{
				bool iInRemoved = false;
				for(size_t j = 0; j < removed.size(); j++)
				{
					if(removed[j] == i)
					{
						iInRemoved = true;
						break;
					}
					if(removed[j] > i)
					{
						break;
					}
				}
				if(iInRemoved)
				{
					i++;
				}
				else
				{
					break;
				}
			}

			if(it2.get(fList) != i + 200)
			{
				testsFailed.push_back(57);
				break;
			}
			i++;
		}

		for(size_t j = 0; j < removed.size(); j++)
		{
			fList.add(removed[j] + 200);
		}
	}

	//test 58 and 59: iterator with no removed data
	{
		//test 58
		auto it1 = fList.getIterator();
		size_t i = 0;
		for(;it1.notDone(); it1.next(fList))
		{
			std::cout << it1.get(fList) << std::endl;
			if(it1.get(fList) != i + 200)
			{
				testsFailed.push_back(58);
				break;
			}
			i++;
		}

		//test 59
		if(it1.shouldGetDiscontigousIterator(fList) != false)
		{
			testsFailed.push_back(59);
		}
	}

	//test 60: case 4,5,8 (p---)

	//test 61: case 4,5,9 (p**---)

	//test 62: case 4,10,11 (-**p-)

	//test 63: case 4,10,12 (-**p**)

	//test 64: case 4,10,11,13 (-p-)

	//test 65: case 4,10,12,13 (-p**)

	if(testsFailed.size() == 0)
	{
		std::cout << "FastInsertList passed all tests" << std::endl;
	}
	else
	{
		if(testsFailed.size() == 1)
		{
			std::cout << "FastInsertList failed test " << testsFailed[0] << std::endl;
		}
		else
		{
			std::cout << "FastInsertList failed tests ";
			for(size_t i = 0; i < testsFailed.size() - 1; i++)
			{
				std::cout << testsFailed[i] << ", ";
			}
			std::cout << "and " << testsFailed[testsFailed.size()-1] << std::endl;
		}
	}
}

int main(int /*argc*/, const char** /*argv*/)
{
	testFinitePQueue();
	testSPSCQueue();
	testList();
	testFastInsertList();

	/*
	const char* baseStr = "哈的是 энергия буран поезд поезда";
	const char* subStr = "поезда";
	const char* letterW = "w";
	const char* testCStr = "поезда";
	const char* startCStr = "哈的是";

	fsds::CompositeString str = fsds::CompositeString(reinterpret_cast<const fsds::CompositeString::CharT*>(baseStr), 30);
	fsds::CompositeString smallStr = fsds::CompositeString(reinterpret_cast<const fsds::CompositeString::CharT*>(subStr), 6);
	fsds::CompositeString singleCharacter = fsds::CompositeString(reinterpret_cast<const fsds::CompositeString::CharT*>(letterW), 1);
	fsds::CompositeString testStr = fsds::CompositeString(reinterpret_cast<const fsds::CompositeString::CharT*>(testCStr), 6);
	fsds::CompositeString startStr = fsds::CompositeString(reinterpret_cast<const fsds::CompositeString::CharT*>(startCStr), 3);

	
	str.set(10, singleCharacter);
	str.append(singleCharacter);
	str.append(smallStr);
	str.append(singleCharacter);
	str.replace(1,2,smallStr);
	str.insert(3, smallStr);
	std::cout << "before replace: " << str << std::endl;
	str.replace(smallStr, singleCharacter);
	std::cout << "after replace: " << str << std::endl;
	

	std::cout << "equ " << (smallStr == testStr) << std::endl;
	//return 0;

	std::cout << "full string: " << str << std::endl;
	
	std::cout << "string character by character: ";
	for(size_t i = 0; i < str.size(); i++)
	{
		std::cout << str[i];
	}
	std::cout <<  std::endl;

	std::cout << "substr: " << str.substr(1,5) << std::endl;

	std::cout << "find: " << str.find(smallStr) << std::endl;

	std::cout << "compare: " << str.compare(smallStr) << std::endl;

	std::cout << "starts with: " << str.startsWith(startStr) << std::endl;
	std::cout << "starts with: " << str.startsWith(smallStr) << std::endl;
	std::cout << "ends with: " << str.endsWith(startStr) << std::endl;
	std::cout << "ends with: " << str.endsWith(smallStr) << std::endl;
	*/

	return 0;
}