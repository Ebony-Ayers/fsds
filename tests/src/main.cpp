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
		auto result = pqueue.dequeue();
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
		pqueue.enqueue(i, i / ((testSize / numPriorities) + 1));
	}
	for(size_t i = 0; i < testSize; i++)
	{
		auto result = pqueue.dequeue();
		if(result != i)
		{
			testsFailed.push_back(5);
			break;
		}
	}
	
	//test 6: enqueue and dequeuePriority on all priorities
	for(size_t i = 0; i < testSize; i++)
	{
		pqueue.enqueue(i, i % numPriorities);
	}
	for(size_t i = 0; i < testSize; i++)
	{
		auto result = pqueue.dequeuePriority(i % numPriorities);
		if(result != i)
		{
			testsFailed.push_back(6);
			break;
		}
	}
	
	//test 7: tryDequeue()
	pqueue.enqueue(100, 0);
	{
		size_t dest;
		if(pqueue.tryDequeue(&dest, 1) != false)
		{
			testsFailed.push_back(7);
		}
		if(pqueue.tryDequeue(&dest, 0) != true)
		{
			testsFailed.push_back(7);
		}
		if(dest != 100)
		{
			testsFailed.push_back(7);
		}
	}

	//test 8: frontPriority()
	pqueue.enqueue(100, 0);
	if(pqueue.frontPriority(0) != 100)
	{
		testsFailed.push_back(8);
	}

	//test 9: isEmpty()
	if(pqueue.isEmpty() != false)
	{
		testsFailed.push_back(9);
	}
	pqueue.clear();
	if(pqueue.isEmpty() != true)
	{
		testsFailed.push_back(9);
	}

	//test 10: enqueueFront
	for(size_t i = 0; i < testSize; i++)
	{
		pqueue.enqueueFront(i, 0);
	}
	for(size_t i = testSize; i > 0; i--)
	{
		if(pqueue.dequeue() != i - 1)
		{
			testsFailed.push_back(10);
			break;
		}
	}

	//test 11: front()
	pqueue.enqueue(10, 2);
	if(pqueue.front() != 10)
	{
		testsFailed.push_back(11);
	}

	//test 12: getCurrentHighestPriority()
	if(pqueue.getCurrentHighestPriority() != 2)
	{
		testsFailed.push_back(12);
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
	
	//test 13: bulk queue dequeue
	for(size_t i = 0; i < 6; i++)
	{
		bool failed = false;
		for(size_t j = 0; j < testSize; j++)
		{
			queue.enqueue(j);
		}
		for(size_t j = 0; j < testSize; j++)
		{
			if(queue.dequeue() != j)
			{
				testsFailed.push_back(13);
				failed = true;
				break;
			}
		}
		if(failed) { break; }
		else if(queue.capacity() != 128)
		{
			testsFailed.push_back(13);
			break;
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

void fastInsertListRunSubTest1(fsds::FastInsertList<size_t>& l, std::vector<size_t> initiallyRemoved, const size_t& toRemove, std::vector<size_t>& testsFailed, const size_t& testNum)
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
		size_t location = l.add(i + 200);
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
		if(l[initiallyRemoved[i]] != i + 200)
		{
			testsFailed.push_back(testNum);
			return;
		}
	}
}

void fastInsertListRunSubTest2(fsds::FastInsertList<size_t>& l, std::vector<size_t> initiallyRemoved, const size_t& toRemove, std::vector<size_t>& testsFailed, const size_t& testNum)
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
		size_t location = l.add(initiallyRemoved[i] + 200);
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
	auto it = l.getIterator();
	size_t i = 0;
	for(; it.notDone(); it.next(l))
	{
		if(it.get(l) != i + 200)
		{
			testsFailed.push_back(testNum);
			return;
		}
		i++;
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

	//set all the values to i+200 so that the subtest function can differentiate the data part of the union from the next part
	for(size_t i = 0; i < fList.size(); i++)
	{
		fList[i] = i + 200;
	}
	
	//see src/fast_insert_list.inl fsds::FastInsertList<T>::remove for notation
	std::vector<size_t> initiallyRemoved;

	//(---p)
	initiallyRemoved = {1, 2};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 7);
	//(p---)
	initiallyRemoved = {2, 3};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 1, testsFailed, 8);
	//(---*p)
	initiallyRemoved = {1, 2};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 4, testsFailed, 9);
	//(p*---)
	initiallyRemoved = {3, 4};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 1, testsFailed, 10);
	//(-*-p)
	initiallyRemoved = {1, 3};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 4, testsFailed, 11);
	//(-*-*p)
	initiallyRemoved = {1, 3};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 5, testsFailed, 12);
	//(p-*-)
	initiallyRemoved = {2, 4};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 1, testsFailed, 13);
	//(p*-*-)
	initiallyRemoved = {3, 5};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 1, testsFailed, 14);
	//(-p-)
	initiallyRemoved = {1, 3};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 2, testsFailed, 15);
	//(-*p*-)
	initiallyRemoved = {1, 5};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 16);
	//(-*p-)
	initiallyRemoved = {1, 4};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 17);
	//(-p*-)
	initiallyRemoved = {1, 4};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 2, testsFailed, 18);
	

	//(---*-p)
	initiallyRemoved = {1, 2, 4};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 5, testsFailed, 19);
	//(---*p-)
	initiallyRemoved = {1, 2, 5};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 4, testsFailed, 20);
	
	//(-p*---)
	initiallyRemoved = {1, 4, 5};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 2, testsFailed, 21);
	//(p-*---)
	initiallyRemoved = {2, 4, 5};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 1, testsFailed, 22);
	//(---*---p)
	initiallyRemoved = {1, 2, 4, 5};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 6, testsFailed, 23);
	//(---*p---)
	initiallyRemoved = {1, 2, 5, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 4, testsFailed, 24);
	//(---p*---)
	initiallyRemoved = {1, 2, 5, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 25);
	//(p---*---)
	initiallyRemoved = {2, 3, 5, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 1, testsFailed, 26);
	//(---p---)
	initiallyRemoved = {1, 2, 4, 5};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 27);
	//(-p---)
	initiallyRemoved = {1, 3, 4};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 2, testsFailed, 28);
	//(---p-)
	initiallyRemoved = {1, 2, 4};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 29);
	//(---*p---)
	initiallyRemoved = {1, 2, 5, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 4, testsFailed, 30);
	//(---p*---)
	initiallyRemoved = {1, 2, 5, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 31);
	//(-*p---)
	initiallyRemoved = {1, 4, 5};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 32);
	//(---p*-)
	initiallyRemoved = {1, 2, 5};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 33);
	//(---*p*---)
	initiallyRemoved = {1, 2, 6, 7};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 5, testsFailed, 34);
	//(-*p*---)
	initiallyRemoved = {1, 5, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 35);
	//(---*p*-)
	initiallyRemoved = {1, 2, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 4, testsFailed, 36);
	//(--*--p--)
	initiallyRemoved = {1, 2, 4, 5, 7, 8};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 6, testsFailed, 37);
	//(--*-p--)
	initiallyRemoved = {1, 2, 4, 6, 7};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 5, testsFailed, 38);
	//(--*--p-)
	initiallyRemoved = {1, 2, 4, 5, 7};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 6, testsFailed, 39);
	//(-*--p--)
	initiallyRemoved = {1, 3, 4, 6, 7};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 5, testsFailed, 40);
	//(-*-p--)
	initiallyRemoved = {1, 3, 5, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 4, testsFailed, 41);
	//(-*--p-)
	initiallyRemoved = {1, 3, 4, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 5, testsFailed, 42);
	//(--p--*--)
	initiallyRemoved = {1, 2, 4, 5, 7, 8};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 43);
	//(--p-*--)
	initiallyRemoved = {1, 2, 4, 6, 7};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 44);
	//(--p--*-)
	initiallyRemoved = {1, 2, 4, 5, 7};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 45);
	//(-p--*--)
	initiallyRemoved = {1, 3, 4, 6, 7};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 2, testsFailed, 46);
	//(-p-*--)
	initiallyRemoved = {1, 3, 5, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 2, testsFailed, 47);
	//(-p--*-)
	initiallyRemoved = {1, 3, 4, 6};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 2, testsFailed, 48);
	//(--*--*--p)
	initiallyRemoved = {1, 2, 4, 5, 7, 8};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 9, testsFailed, 49);
	//(--*--*--*p)
	initiallyRemoved = {1, 2, 4, 5, 7, 8};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 10, testsFailed, 50);
	//(p--*--*--)
	initiallyRemoved = {2, 3, 5, 6, 8, 9};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 1, testsFailed, 51);
	//(p*--*--*--)
	initiallyRemoved = {3, 4, 6, 7, 9, 10};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 1, testsFailed, 52);
	//(--*--*--p--)
	initiallyRemoved = {1, 2, 4, 5, 7, 8, 10, 11};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 9, testsFailed, 53);
	//(--*--p--*--)
	initiallyRemoved = {1, 2, 4, 5, 7, 8, 10, 11};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 6, testsFailed, 53);
	//(--p--*--*--)
	initiallyRemoved = {1, 2, 4, 5, 7, 8, 10, 11};
	fastInsertListRunSubTest1(fList, initiallyRemoved, 3, testsFailed, 53);

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
	initiallyRemoved = {4, 5};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 3, testsFailed, 60);

	//test 61: case 4,5,9 (p**---)
	initiallyRemoved = {4, 5};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 2, testsFailed, 61);

	//test 62: case 4,10,11 (-**p-)
	initiallyRemoved = {4, 7};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 6, testsFailed, 62);

	//test 63: case 4,10,12 (-**p**)
	initiallyRemoved = {4};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 6, testsFailed, 63);

	//test 64: case 4,10,11,13 (-p-)
	initiallyRemoved = {4, 6};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 5, testsFailed, 64);

	//test 65: case 4,10,12,13 (-p**)
	initiallyRemoved = {4};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 5, testsFailed, 65);

	//test 66: case 15, 16, 17 (-*-*p--)
	initiallyRemoved = {1, 4, 6, 9, 10};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 8, testsFailed, 66);

	//test 67: case 15, 16, 18 (-*-*p)
	initiallyRemoved = {1, 4, 6};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 8, testsFailed, 67);

	//test 68: case 15, 16, 17, 19 (-*-p--)
	initiallyRemoved = {1, 4, 6, 8, 9};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 7, testsFailed, 68);
	
	//test 69: case 15, 16, 18, 19 (-*-p)
	initiallyRemoved = {1, 4, 6};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 7, testsFailed, 69);

	//test 70: case 15, 16, 17, 19, 20 (--p--)
	initiallyRemoved = {1, 4, 5, 7, 8};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 6, testsFailed, 70);
	
	//test 71: case 15, 16, 18, 19, 20 (--p)
	initiallyRemoved = {1, 4, 5};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 6, testsFailed, 71);

	//test 72: case 23, 24 (-*p-)
	initiallyRemoved = {1, 4, 7};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 6, testsFailed, 72);

	//test 73: case 23, 25 (-*p*-)
	initiallyRemoved = {1, 4, 8};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 6, testsFailed, 73);

	//test 74: case 23, 24, 26 (-p-)
	initiallyRemoved = {1, 4, 6};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 5, testsFailed, 74);

	//test 75: case 23, 25, 26 (-p*-)
	initiallyRemoved = {1, 4, 7};
	fastInsertListRunSubTest2(fList, initiallyRemoved, 5, testsFailed, 75);

	for(size_t i = 0; i < fList.size(); i++)
	{
		fList[i] = i + 200;
	}
	fsds::FastInsertList<size_t> fList2;
	for(size_t i = 0; i < 20; i++)
	{
		fList2.add(i + 200);
	}
	for(size_t i = 20; i < fList.size(); i++)
	{
		fList2.add(i + 199);
	}
	fList2.remove(20);

	//test 76: value equality with an element removed from the second list but same sequence
	if(!fList.valueEquality(fList2))
	{
		testsFailed.push_back(76);
	}
	//test 77: value equality with an element removed from the second list with different sequence
	fList2[50] = 0;
	if(fList.valueEquality(fList2))
	{
		testsFailed.push_back(77);
	}

	fList2.remove(100);
	fList2.add(0);
	for(size_t i = 0; i < fList.size(); i++)
	{
		fList2[i] = i + 200;
	}
	for(size_t i = 20; i < fList2.size(); i++)
	{
		fList[i] = i + 199;
	}
	fList.remove(20);
	
	//test 78: value equality with an element removed from the first list but same sequence
	if(!fList.valueEquality(fList2))
	{
		testsFailed.push_back(78);
	}
	//test 79: value equality with an element removed from the first list with different sequence
	fList2[50] = 0;
	if(fList.valueEquality(fList2))
	{
		testsFailed.push_back(79);
	}
	
	//test 80: value equality with an element removed from both lists
	fList2.add(0);
	for(size_t i = 30; i < fList2.size(); i++)
	{
		fList2[i] = i + 199;
	}
	fList2.remove(30);

	if(!fList.valueEquality(fList2))
	{
		testsFailed.push_back(80);
	}

	//test 81: value equality with no elements removed in either list
	fList.add(0);
	fList2.add(0);
	for(size_t i = 0; i < fList.size(); i++)
	{
		fList[i] = i + 200;
		fList2[i] = i + 200;
	}

	if(!fList.valueEquality(fList2))
	{
		testsFailed.push_back(81);
	}

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

void testStaticString()
{
	std::vector<size_t> testsFailed;
	
	//english has one code point per character
	const char* baseStringEnglish1 = "this string contains highly meaningful text";
	const char* baseStringEnglish2 = "x";
	const char* baseStringEnglish3 = "this";
	const char* baseStringEnglish4 = "this string contains really meaningful text";
	const char* baseStringEnglish5 = "text";
	const char* baseStringEnglish6 = "st";
	//russian has two code points per character
	const char* baseStringRussian1 = "утф-8 очень раздражает";
	const char* baseStringRussian2 = "е";
	const char* baseStringRussian3 = "утф-8";
	const char* baseStringRussian4 = "утф не очен раздражает";
	const char* baseStringRussian5 = "раздражает";
	const char* baseStringRussian6 = "ет";
	//chinese has 3+ code points per character
	const char* baseStringChinese1 = "希望这被正确翻译";
	const char* baseStringChinese2 = "翻";
	const char* baseStringChinese3 = "希望";
	const char* baseStringChinese4 = "请让这完全有意义";
	const char* baseStringChinese5 = "翻译";
	const char* baseStringChinese6 = "翻被";
	//mixed language strings with various numbers of code points per character
	const char* baseStringMixed1 = "this не 说得通";
	const char* baseStringMixed2 = "得";
	const char* baseStringMixed3 = "s не 说";
	const char* baseStringMixed4 = "that не 说得通";

	const char* baseEmptyString = "";

	fsds::StaticString english1(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringEnglish1), 43);
	fsds::StaticString english2(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringEnglish2), 1);
	fsds::StaticString english3(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringEnglish3), 4);
	fsds::StaticString english4(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringEnglish4), 43);
	fsds::StaticString english5(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringEnglish5), 4);
	fsds::StaticString english6(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringEnglish6), 2);
	fsds::StaticString russian1(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringRussian1), 22);
	fsds::StaticString russian2(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringRussian2), 1);
	fsds::StaticString russian3(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringRussian3), 5);
	fsds::StaticString russian4(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringRussian4), 22);
	fsds::StaticString russian5(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringRussian5), 10);
	fsds::StaticString russian6(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringRussian6), 2);
	fsds::StaticString chinese1(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringChinese1), 8);
	fsds::StaticString chinese2(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringChinese2), 1);
	fsds::StaticString chinese3(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringChinese3), 2);
	fsds::StaticString chinese4(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringChinese4), 8);
	fsds::StaticString chinese5(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringChinese5), 2);
	fsds::StaticString chinese6(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringChinese6), 2);
	fsds::StaticString mixed1(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringMixed1), 11);
	fsds::StaticString mixed2(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringMixed2), 1);
	fsds::StaticString mixed3(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringMixed3), 6);
	fsds::StaticString mixed4(reinterpret_cast<const fsds::StaticString::CharT*>(baseStringMixed4), 11);
	fsds::StaticString empty(reinterpret_cast<const fsds::StaticString::CharT*>(baseEmptyString), 0);

	//test 1: equality
	{
		bool failed = false;
		if(english1 != english1)
		{
			failed = true;
		}
		else if(russian1 != russian1)
		{
			failed = true;
		}
		else if(chinese1 != chinese1)
		{
			failed = true;
		}
		else if(english1 == english2)
		{
			failed = true;
		}
		else if(mixed1 != mixed1)
		{
			failed = true;
		}
		else if(russian1 == russian2)
		{
			failed = true;
		}
		else if(chinese1 == chinese2)
		{
			failed = true;
		}
		else if(mixed1 == mixed2)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(1);
		}
	}

	//test 2: operator[]
	{
		bool failed = false;
		if(english1[41] != english2)
		{
			failed = true;
		}
		else if(russian1[20] != russian2)
		{
			failed = true;
		}
		else if(chinese1[6] != chinese2)
		{
			failed = true;
		}
		else if(mixed1[9] != mixed2)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(2);
		}
	}

	//test 3: size()
	{
		bool failed = false;
		if(english1.size() != 43)
		{
			failed = true;
		}
		else if(russian1.size() != 22)
		{
			failed = true;
		}
		else if(chinese1.size() != 8)
		{
			failed = true;
		}
		else if(mixed1.size() != 11)
		{
			failed = true;
		}
		else if(english2.size() != 1)
		{
			failed = true;
		}
		else if(russian2.size() != 1)
		{
			failed = true;
		}
		else if(chinese2.size() != 1)
		{
			failed = true;
		}
		else if(mixed2.size() != 1)
		{
			failed = true;
		}
		else if(empty.size() != 0)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(3);
		}
	}

	//test 4: numCodePointsInString()
	{
		bool failed = false;
		if(english1.numCodePointsInString() != 43)
		{
			failed = true;
		}
		else if(russian1.numCodePointsInString() != 40)
		{
			failed = true;
		}
		else if(chinese1.numCodePointsInString() != 24)
		{
			failed = true;
		}
		else if(mixed1.numCodePointsInString() != 19)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(4);
		}
	}

	//test 5: isEmpty()
	{
		bool failed = false;
		if(english1.isEmpty() != false)
		{
			failed = true;
		}
		else if(russian1.isEmpty() != false)
		{
			failed = true;
		}
		else if(chinese1.isEmpty() != false)
		{
			failed = true;
		}
		else if(mixed1.isEmpty() != false)
		{
			failed = true;
		}
		else if(empty.isEmpty() != true)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(5);
		}
	}

	//test 6: substr
	{
		bool failed = false;
		if(english1.substr(0,4) != english3)
		{
			failed = true;
		}
		else if(russian1.substr(0,5) != russian3)
		{
			failed = true;
		}
		else if(chinese1.substr(0,2) != chinese3)
		{
			failed = true;
		}
		else if(mixed1.substr(3,6) != mixed3)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(6);
		}
	}

	//test 7: compare
	{
		bool failed = false;
		if(english1.compare(english4) != 1)
		{
			failed = true;
		}
		else if(russian1.compare(russian4) != -1)
		{
			failed = true;
		}
		else if(chinese1.compare(chinese4) != 1)
		{
			failed = true;
		}
		else if(mixed1.compare(mixed4) != -1)
		{
			failed = true;
		}
		else if(english1.compare(english3) != -1)
		{
			failed = true;
		}
		else if(russian1.compare(russian3) != -1)
		{
			failed = true;
		}
		else if(chinese1.compare(chinese3) != -1)
		{
			failed = true;
		}
		else if(mixed1.compare(mixed3) != -1)
		{
			failed = true;
		}
		else if(english1.compare(english1) != 0)
		{
			failed = true;
		}
		else if(russian1.compare(russian1) != 0)
		{
			failed = true;
		}
		else if(chinese1.compare(chinese1) != 0)
		{
			failed = true;
		}
		else if(mixed1.compare(mixed1) != 0)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(7);
		}
	}

	//test 8: contains
	{
		bool failed = false;
		if(english1.contains(english3) != true)
		{
			failed = true;
		}
		else if(russian1.contains(russian3) != true)
		{
			failed = true;
		}
		else if(chinese1.contains(chinese3) != true)
		{
			failed = true;
		}
		else if(mixed1.contains(mixed3) != true)
		{
			failed = true;
		}
		else if(english1.contains(chinese3) != false)
		{
			failed = true;
		}
		else if(russian1.contains(mixed3) != false)
		{
			failed = true;
		}
		else if(chinese1.contains(mixed3) != false)
		{
			failed = true;
		}
		else if(mixed1.contains(russian3) != false)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(8);
		}
	}

	//test 9: find
	{
		bool failed = false;
		if(english1.find(english3) != 0)
		{
			failed = true;
		}
		else if(russian1.find(russian3) != 0)
		{
			failed = true;
		}
		else if(chinese1.find(chinese3) != 0)
		{
			failed = true;
		}
		else if(mixed1.find(mixed3) != 3)
		{
			failed = true;
		}
		else if(english1.find(chinese3) != fsds::StaticString::npos)
		{
			failed = true;
		}
		else if(russian1.find(mixed3) != fsds::StaticString::npos)
		{
			failed = true;
		}
		else if(chinese1.find(mixed3) != fsds::StaticString::npos)
		{
			failed = true;
		}
		else if(mixed1.find(russian3) != fsds::StaticString::npos)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(9);
		}
	}

	//test 10: findItterator
	{
		bool failed = false;
		{
			fsds::StaticStringItterator it = english1.findItterator(english3);
			if((it.str() != &english1) || (it.currentPosition() != 0) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator it = russian1.findItterator(russian3);
			if((it.str() != &russian1) || (it.currentPosition() != 0) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator it = chinese1.findItterator(chinese3);
			if((it.str() != &chinese1) || (it.currentPosition() != 0) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator it = mixed1.findItterator(mixed3);
			if((it.str() != &mixed1) || (it.currentPosition() != 3) || (it.currentCodePointOffset() != 3))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator it = english1.findItterator(chinese3);
			if((it.str() != &english1) || (it.currentPosition() != fsds::StaticString::npos) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator it = russian1.findItterator(mixed3);
			if((it.str() != &russian1) || (it.currentPosition() != fsds::StaticString::npos) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator it = chinese1.findItterator(mixed3);
			if((it.str() != &chinese1) || (it.currentPosition() != fsds::StaticString::npos) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator it = mixed1.findItterator(russian3);
			if((it.str() != &mixed1) || (it.currentPosition() != fsds::StaticString::npos) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		} 

		if(failed)
		{
			testsFailed.push_back(10);
		}
	}
	
	//test 11: findAnyCharacter
	{
		bool failed = false;
		if(english1.findAnyCharacter(english5) != 0)
		{
			failed = true;
		}
		else if(russian1.findAnyCharacter(russian5) != 1)
		{
			failed = true;
		}
		else if(chinese1.findAnyCharacter(chinese5) != 6)
		{
			failed = true;
		}
		else if(mixed1.findAnyCharacter(mixed4) != 0)
		{
			failed = true;
		}
		else if(english1.findAnyCharacter(chinese3) != fsds::StaticString::npos)
		{
			failed = true;
		}
		else if(russian1.findAnyCharacter(mixed3) != 5)
		{
			failed = true;
		}
		else if(chinese1.findAnyCharacter(mixed3) != fsds::StaticString::npos)
		{
			failed = true;
		}
		else if(mixed1.findAnyCharacter(russian3) != fsds::StaticString::npos)
		{
			failed = true;
		}
		else if(english1.findAnyCharacter(english6) != 0)
		{
			failed = true;
		}
		else if(russian1.findAnyCharacter(russian6) != 1)
		{
			failed = true;
		}
		else if(chinese1.findAnyCharacter(chinese6) != 3)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(11);
		}
	}
	//test 12: findAnyCharacterItterator
	{
		bool failed = false;
		{
			fsds::StaticStringItterator iterator = english1.findAnyCharacterItterator(english5);
			if((iterator.str() != &english1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = russian1.findAnyCharacterItterator(russian5);
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 1) || (iterator.currentCodePointOffset() != 2))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = chinese1.findAnyCharacterItterator(chinese5); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != 6) || (iterator.currentCodePointOffset() != 18))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = mixed1.findAnyCharacterItterator(mixed4); 
			if((iterator.str() != &mixed1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = english1.findAnyCharacterItterator(chinese3); 
			if((iterator.str() != &english1) || (iterator.currentPosition() != fsds::StaticString::npos) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = russian1.findAnyCharacterItterator(mixed3); 
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 5) || (iterator.currentCodePointOffset() != 8))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = chinese1.findAnyCharacterItterator(mixed3); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != fsds::StaticString::npos) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = mixed1.findAnyCharacterItterator(russian3); 
			if((iterator.str() != &mixed1) || (iterator.currentPosition() != fsds::StaticString::npos) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = english1.findAnyCharacterItterator(english6); 
			if((iterator.str() != &english1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = russian1.findAnyCharacterItterator(russian6); 
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 1) || (iterator.currentCodePointOffset() != 2))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = chinese1.findAnyCharacterItterator(chinese6); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != 3) || (iterator.currentCodePointOffset() != 9))
			{
				failed = true;
			}
		}

		if(failed)
		{
			testsFailed.push_back(12);
		}
	}

	//test 13: startsWith
	{
		bool failed = false;
		if(english1.startsWith(english3) != true)
		{
			failed = true;
		}
		else if(russian1.startsWith(russian3) != true)
		{
			failed = true;
		}
		else if(chinese1.startsWith(chinese3) != true)
		{
			failed = true;
		}
		else if(mixed1.startsWith(mixed3) != false)
		{
			failed = true;
		}
		else if(english1.startsWith(chinese3) != false)
		{
			failed = true;
		}
		else if(russian1.startsWith(mixed3) != false)
		{
			failed = true;
		}
		else if(chinese1.startsWith(mixed3) != false)
		{
			failed = true;
		}
		else if(mixed1.startsWith(russian3) != false)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(13);
		}
	}

	//test 14: endsWith
	{
		bool failed = false;
		if(english1.endsWith(english5) != true)
		{
			failed = true;
		}
		else if(russian1.endsWith(russian5) != true)
		{
			failed = true;
		}
		else if(chinese1.endsWith(chinese5) != true)
		{
			failed = true;
		}
		else if(mixed1.endsWith(mixed3) != false)
		{
			failed = true;
		}
		else if(english1.endsWith(chinese3) != false)
		{
			failed = true;
		}
		else if(russian1.endsWith(mixed3) != false)
		{
			failed = true;
		}
		else if(chinese1.endsWith(mixed3) != false)
		{
			failed = true;
		}
		else if(mixed1.endsWith(russian3) != false)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(14);
		}
	}

	//test 15: numCodePointsInFirstCharacter
	{
		bool failed = false;
		if(english1.numCodePointsInFirstCharacter() != 1)
		{
			failed = true;
		}
		else if(russian1.numCodePointsInFirstCharacter() != 2)
		{
			failed = true;
		}
		else if(chinese1.numCodePointsInFirstCharacter() != 3)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(15);
		}
	}

	if(testsFailed.size() == 0)
	{
		std::cout << "StaticString passed all tests" << std::endl;
	}
	else
	{
		if(testsFailed.size() == 1)
		{
			std::cout << "StaticString failed test " << testsFailed[0] << std::endl;
		}
		else
		{
			std::cout << "StaticString failed tests ";
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
	testSPSCQueue();
	testFinitePQueue();
	testList();
	testFastInsertList();
	testStaticString();

	return 0;
}