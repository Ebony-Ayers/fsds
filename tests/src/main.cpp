#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <vector>
#include <source_location>
#include <algorithm>
#include <climits>

#define FSDS_DEBUG
//#define ALLIGNED_ALLOCATOR_PRINT_ALLOCATIONS
//#define ALLIGNED_RAW_ALLOCATOR_PRINT_ALLOCATIONS
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

void testSeperateDataList()
{
	std::vector<size_t> testsFailed;

	size_t testSize = 100;
	fsds::SeperateDataList<size_t> list;

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

	//test 4: size() and isEmpty()
	if((list.size() != 100) || (list.isEmpty() == true))
	{
		testsFailed.push_back(4);
	}

	//test 5: clear()
	list.clear();
	if((list.size() != 0) || (list.isEmpty() == false))
	{
		testsFailed.push_back(5);
	}

	//test 6: reserve()
	list.reserve(200);
	if(list.capacity() < 200)
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
			std::cout << list[i] << " != " << (testSize - i - 1) << std::endl;
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
	fsds::SeperateDataList<placeHolderClass> list2;
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
	fsds::SeperateDataList<size_t> list3;
	fsds::SeperateDataList<size_t> list4;
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
	fsds::SeperateDataList<size_t> list5 = list;
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
	fsds::SeperateDataList<size_t> list6 = std::move(list5);
	if((list6 != list4) || (list5.data() != nullptr))
	{
		testsFailed.push_back(17);
	}
	
	//test 18: externalReallocate
	{
		size_t minSize = list6.getExternalReallocateMinimumRequiredSpace() / sizeof(size_t);
		std::allocator<size_t> alloc;
		size_t* ptr = alloc.allocate(minSize*2);
		list6.externalReallocate(ptr, minSize*2);
		if(list6 != list4)
		{
			testsFailed.push_back(17);
		}
	}

	if(testsFailed.size() == 0)
	{
		std::cout << "SeperateDataList passed all tests" << std::endl;
	}
	else
	{
		if(testsFailed.size() == 1)
		{
			std::cout << "SeperateDataList failed test " << testsFailed[0] << std::endl;
		}
		else
		{
			std::cout << "SeperateDataList failed tests ";
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

	//test 13: findNotAnyCharacter
	{
		bool failed = false;
		if(english1.findNotAnyCharacter(english5) != 1)
		{
			failed = true;
		}
		else if(russian1.findNotAnyCharacter(russian5) != 0)
		{
			failed = true;
		}
		else if(chinese1.findNotAnyCharacter(chinese5) != 0)
		{
			failed = true;
		}
		else if(mixed1.findNotAnyCharacter(mixed4) != 2)
		{
			failed = true;
		}
		else if(english1.findNotAnyCharacter(chinese3) != 0)
		{
			failed = true;
		}
		else if(russian1.findNotAnyCharacter(mixed3) != 0)
		{
			failed = true;
		}
		else if(chinese1.findNotAnyCharacter(mixed3) != 0)
		{
			failed = true;
		}
		else if(mixed1.findNotAnyCharacter(russian3) != 0)
		{
			failed = true;
		}
		else if(english1.findNotAnyCharacter(english6) != 1)
		{
			failed = true;
		}
		else if(russian1.findNotAnyCharacter(russian6) != 0)
		{
			failed = true;
		}
		else if(chinese1.findNotAnyCharacter(chinese6) != 0)
		{
			failed = true;
		}
		else if(english1.findNotAnyCharacter(english1) != fsds::StaticString::npos)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(13);
		}
	}
	
	//test 14: findAnyCharacterItterator
	{
		bool failed = false;
		{
			fsds::StaticStringItterator iterator = english1.findNotAnyCharacterItterator(english5);
			if((iterator.str() != &english1) || (iterator.currentPosition() != 1) || (iterator.currentCodePointOffset() != 1))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = russian1.findNotAnyCharacterItterator(russian5);
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = chinese1.findNotAnyCharacterItterator(chinese5); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = mixed1.findNotAnyCharacterItterator(mixed4); 
			if((iterator.str() != &mixed1) || (iterator.currentPosition() != 2) || (iterator.currentCodePointOffset() != 2))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = english1.findNotAnyCharacterItterator(chinese3); 
			if((iterator.str() != &english1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = russian1.findNotAnyCharacterItterator(mixed3); 
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = chinese1.findNotAnyCharacterItterator(mixed3); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = mixed1.findNotAnyCharacterItterator(russian3); 
			if((iterator.str() != &mixed1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = english1.findNotAnyCharacterItterator(english6); 
			if((iterator.str() != &english1) || (iterator.currentPosition() != 1) || (iterator.currentCodePointOffset() != 1))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = russian1.findNotAnyCharacterItterator(russian6); 
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = chinese1.findNotAnyCharacterItterator(chinese6); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::StaticStringItterator iterator = english1.findNotAnyCharacterItterator(english1); 
			if((iterator.str() != &english1) || (iterator.isNPos() != true))
			{
				failed = true;
			}
		}

		if(failed)
		{
			testsFailed.push_back(14);
		}
	}

	//test 15: startsWith
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
			testsFailed.push_back(15);
		}
	}

	//test 16: endsWith
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
			testsFailed.push_back(16);
		}
	}

	//test 17: numCodePointsInFirstCharacter
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
			testsFailed.push_back(17);
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

void testDynamicString()
{
	std::vector<size_t> testsFailed;
	
	//english has one code point per character
	const char* baseStringEnglish1 = "this string contains highly meaningful text";
	const char* baseStringEnglish2 = "x";
	const char* baseStringEnglish3 = "this";
	const char* baseStringEnglish4 = "this string contains really meaningful text";
	const char* baseStringEnglish5 = "text";
	const char* baseStringEnglish6 = "st";
	const char* baseStringEnglish7 = "aaaa";
	const char* baseStringEnglish8 = "bb";
	const char* baseStringEnglish9 = "aabbaa";
	const char* baseStringEnglish10 = "bbaaaa";
	const char* baseStringEnglish11 = "aaaabb";
	const char* baseStringEnglish12 = "aaaaaaaa";
	//russian has two code points per character
	const char* baseStringRussian1 = "утф-8 очень раздражает";
	const char* baseStringRussian2 = "е";
	const char* baseStringRussian3 = "утф-8";
	const char* baseStringRussian4 = "утф не очен раздражает";
	const char* baseStringRussian5 = "раздражает";
	const char* baseStringRussian6 = "ет";
	const char* baseStringRussian7 = "аааа";
	const char* baseStringRussian8 = "вв";
	const char* baseStringRussian9 = "аавваа";
	const char* baseStringRussian10 = "вваааа";
	const char* baseStringRussian11 = "аааавв";
	const char* baseStringRussian12 = "аааааааа";
	//chinese has 3+ code points per character
	const char* baseStringChinese1 = "希望这被正确翻译";
	const char* baseStringChinese2 = "翻";
	const char* baseStringChinese3 = "希望";
	const char* baseStringChinese4 = "请让这完全有意义";
	const char* baseStringChinese5 = "翻译";
	const char* baseStringChinese6 = "翻被";
	const char* baseStringChinese7 = "希希希希";
	const char* baseStringChinese8 = "被被";
	const char* baseStringChinese9 = "希希被被希希";
	const char* baseStringChinese10 = "被被希希希希";
	const char* baseStringChinese11 = "希希希希被被";
	const char* baseStringChinese12 = "希希希希希希希希";
	//mixed language strings with various numbers of code points per character
	const char* baseStringMixed1 = "this не 说得通";
	const char* baseStringMixed2 = "得";
	const char* baseStringMixed3 = "s не 说";
	const char* baseStringMixed4 = "that не 说得通";
	const char* baseStringMixed5 = "this не";
	const char* baseStringMixed6 = "被被";
	const char* baseStringMixed7 = "this被被 не";
	const char* baseStringMixed8 = "被被this не";
	const char* baseStringMixed9 = "this не被被";

	const char* baseEmptyString = "";

	fsds::DynamicString english1(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish1), 43);
	fsds::DynamicString english2(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish2), 1);
	fsds::DynamicString english3(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish3), 4);
	fsds::DynamicString english4(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish4), 43);
	fsds::DynamicString english5(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish5), 4);
	fsds::DynamicString english6(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish6), 2);
	fsds::DynamicString english7(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish7), 4);
	fsds::DynamicString english8(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish8), 2);
	fsds::DynamicString english9(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish9), 6);
	fsds::DynamicString english10(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish10), 6);
	fsds::DynamicString english11(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish11), 6);
	fsds::DynamicString english12(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringEnglish12), 8);
	fsds::DynamicString russian1(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian1), 22);
	fsds::DynamicString russian2(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian2), 1);
	fsds::DynamicString russian3(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian3), 5);
	fsds::DynamicString russian4(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian4), 22);
	fsds::DynamicString russian5(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian5), 10);
	fsds::DynamicString russian6(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian6), 2);
	fsds::DynamicString russian7(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian7), 4);
	fsds::DynamicString russian8(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian8), 2);
	fsds::DynamicString russian9(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian9), 6);
	fsds::DynamicString russian10(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian10), 6);
	fsds::DynamicString russian11(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian11), 6);
	fsds::DynamicString russian12(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringRussian12), 8);
	fsds::DynamicString chinese1(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese1), 8);
	fsds::DynamicString chinese2(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese2), 1);
	fsds::DynamicString chinese3(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese3), 2);
	fsds::DynamicString chinese4(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese4), 8);
	fsds::DynamicString chinese5(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese5), 2);
	fsds::DynamicString chinese6(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese6), 2);
	fsds::DynamicString chinese7(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese7), 4);
	fsds::DynamicString chinese8(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese8), 2);
	fsds::DynamicString chinese9(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese9), 6);
	fsds::DynamicString chinese10(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese10), 6);
	fsds::DynamicString chinese11(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese11), 6);
	fsds::DynamicString chinese12(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringChinese12), 8);
	fsds::DynamicString mixed1(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringMixed1), 11);
	fsds::DynamicString mixed2(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringMixed2), 1);
	fsds::DynamicString mixed3(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringMixed3), 6);
	fsds::DynamicString mixed4(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringMixed4), 11);
	fsds::DynamicString mixed5(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringMixed5), 7);
	fsds::DynamicString mixed6(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringMixed6), 2);
	fsds::DynamicString mixed7(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringMixed7), 9);
	fsds::DynamicString mixed8(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringMixed8), 9);
	fsds::DynamicString mixed9(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStringMixed9), 9);
	fsds::DynamicString empty(reinterpret_cast<const fsds::DynamicString::CharT*>(baseEmptyString), 0);

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
		else if(english1.find(chinese3) != fsds::DynamicString::npos)
		{
			failed = true;
		}
		else if(russian1.find(mixed3) != fsds::DynamicString::npos)
		{
			failed = true;
		}
		else if(chinese1.find(mixed3) != fsds::DynamicString::npos)
		{
			failed = true;
		}
		else if(mixed1.find(russian3) != fsds::DynamicString::npos)
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
			fsds::DynamicStringItterator it = english1.findItterator(english3);
			if((it.str() != &english1) || (it.currentPosition() != 0) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator it = russian1.findItterator(russian3);
			if((it.str() != &russian1) || (it.currentPosition() != 0) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator it = chinese1.findItterator(chinese3);
			if((it.str() != &chinese1) || (it.currentPosition() != 0) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator it = mixed1.findItterator(mixed3);
			if((it.str() != &mixed1) || (it.currentPosition() != 3) || (it.currentCodePointOffset() != 3))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator it = english1.findItterator(chinese3);
			if((it.str() != &english1) || (it.currentPosition() != fsds::DynamicString::npos) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator it = russian1.findItterator(mixed3);
			if((it.str() != &russian1) || (it.currentPosition() != fsds::DynamicString::npos) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator it = chinese1.findItterator(mixed3);
			if((it.str() != &chinese1) || (it.currentPosition() != fsds::DynamicString::npos) || (it.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator it = mixed1.findItterator(russian3);
			if((it.str() != &mixed1) || (it.currentPosition() != fsds::DynamicString::npos) || (it.currentCodePointOffset() != 0))
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
		else if(english1.findAnyCharacter(chinese3) != fsds::DynamicString::npos)
		{
			failed = true;
		}
		else if(russian1.findAnyCharacter(mixed3) != 5)
		{
			failed = true;
		}
		else if(chinese1.findAnyCharacter(mixed3) != fsds::DynamicString::npos)
		{
			failed = true;
		}
		else if(mixed1.findAnyCharacter(russian3) != fsds::DynamicString::npos)
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
			fsds::DynamicStringItterator iterator = english1.findAnyCharacterItterator(english5);
			if((iterator.str() != &english1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = russian1.findAnyCharacterItterator(russian5);
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 1) || (iterator.currentCodePointOffset() != 2))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = chinese1.findAnyCharacterItterator(chinese5); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != 6) || (iterator.currentCodePointOffset() != 18))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = mixed1.findAnyCharacterItterator(mixed4); 
			if((iterator.str() != &mixed1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = english1.findAnyCharacterItterator(chinese3); 
			if((iterator.str() != &english1) || (iterator.currentPosition() != fsds::DynamicString::npos) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = russian1.findAnyCharacterItterator(mixed3); 
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 5) || (iterator.currentCodePointOffset() != 8))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = chinese1.findAnyCharacterItterator(mixed3); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != fsds::DynamicString::npos) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = mixed1.findAnyCharacterItterator(russian3); 
			if((iterator.str() != &mixed1) || (iterator.currentPosition() != fsds::DynamicString::npos) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = english1.findAnyCharacterItterator(english6); 
			if((iterator.str() != &english1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = russian1.findAnyCharacterItterator(russian6); 
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 1) || (iterator.currentCodePointOffset() != 2))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = chinese1.findAnyCharacterItterator(chinese6); 
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

	//test 13: findNotAnyCharacter
	{
		bool failed = false;
		if(english1.findNotAnyCharacter(english5) != 1)
		{
			failed = true;
		}
		else if(russian1.findNotAnyCharacter(russian5) != 0)
		{
			failed = true;
		}
		else if(chinese1.findNotAnyCharacter(chinese5) != 0)
		{
			failed = true;
		}
		else if(mixed1.findNotAnyCharacter(mixed4) != 2)
		{
			failed = true;
		}
		else if(english1.findNotAnyCharacter(chinese3) != 0)
		{
			failed = true;
		}
		else if(russian1.findNotAnyCharacter(mixed3) != 0)
		{
			failed = true;
		}
		else if(chinese1.findNotAnyCharacter(mixed3) != 0)
		{
			failed = true;
		}
		else if(mixed1.findNotAnyCharacter(russian3) != 0)
		{
			failed = true;
		}
		else if(english1.findNotAnyCharacter(english6) != 1)
		{
			failed = true;
		}
		else if(russian1.findNotAnyCharacter(russian6) != 0)
		{
			failed = true;
		}
		else if(chinese1.findNotAnyCharacter(chinese6) != 0)
		{
			failed = true;
		}
		else if(english1.findNotAnyCharacter(english1) != fsds::DynamicString::npos)
		{
			failed = true;
		}

		if(failed)
		{
			testsFailed.push_back(13);
		}
	}
	
	//test 14: findAnyCharacterItterator
	{
		bool failed = false;
		{
			fsds::DynamicStringItterator iterator = english1.findNotAnyCharacterItterator(english5);
			if((iterator.str() != &english1) || (iterator.currentPosition() != 1) || (iterator.currentCodePointOffset() != 1))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = russian1.findNotAnyCharacterItterator(russian5);
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = chinese1.findNotAnyCharacterItterator(chinese5); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = mixed1.findNotAnyCharacterItterator(mixed4); 
			if((iterator.str() != &mixed1) || (iterator.currentPosition() != 2) || (iterator.currentCodePointOffset() != 2))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = english1.findNotAnyCharacterItterator(chinese3); 
			if((iterator.str() != &english1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = russian1.findNotAnyCharacterItterator(mixed3); 
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = chinese1.findNotAnyCharacterItterator(mixed3); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = mixed1.findNotAnyCharacterItterator(russian3); 
			if((iterator.str() != &mixed1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = english1.findNotAnyCharacterItterator(english6); 
			if((iterator.str() != &english1) || (iterator.currentPosition() != 1) || (iterator.currentCodePointOffset() != 1))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = russian1.findNotAnyCharacterItterator(russian6); 
			if((iterator.str() != &russian1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = chinese1.findNotAnyCharacterItterator(chinese6); 
			if((iterator.str() != &chinese1) || (iterator.currentPosition() != 0) || (iterator.currentCodePointOffset() != 0))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicStringItterator iterator = english1.findNotAnyCharacterItterator(english1); 
			if((iterator.str() != &english1) || (iterator.isNPos() != true))
			{
				failed = true;
			}
		}

		if(failed)
		{
			testsFailed.push_back(14);
		}
	}

	//test 15: startsWith
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
			testsFailed.push_back(15);
		}
	}

	//test 16: endsWith
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
			testsFailed.push_back(16);
		}
	}

	//test 17: numCodePointsInFirstCharacter
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
			testsFailed.push_back(17);
		}
	}

	//test 18: insert
	{
		bool failed = false;
		
		{
			fsds::DynamicString str(english7);
			str.insert(2, english8);
			if(str != english9)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(english7);
			str.insert(0, english8);
			if(str != english10)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(english7);
			str.insert(4, english8);
			if(str != english11)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(russian7);
			str.insert(2, russian8);
			if(str != russian9)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(russian7);
			str.insert(0, russian8);
			if(str != russian10)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(russian7);
			str.insert(4, russian8);
			if(str != russian11)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(chinese7);
			str.insert(2, chinese8);
			if(str != chinese9)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(chinese7);
			str.insert(0, chinese8);
			if(str != chinese10)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(chinese7);
			str.insert(4, chinese8);
			if(str != chinese11)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(mixed5);
			str.insert(4, mixed6);
			if(str != mixed7)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(mixed5);
			str.insert(0, mixed6);
			if(str != mixed8)
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(mixed5);
			str.insert(7, mixed6);
			if(str != mixed9)
			{
				failed = true;
			}
		}

		if(failed)
		{
			testsFailed.push_back(18);
		}
	}

	//test 19: replace start end
	{
		bool failed = false;

		{
			fsds::DynamicString str(english9);
			str.replace(2, 4, empty);
			if((str != english7) || (str.size() != 4) || (str.numCodePointsInString() != 4))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(english10);
			str.replace(0, 2, empty);
			if((str != english7) || (str.size() != 4) || (str.numCodePointsInString() != 4))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(english11);
			str.replace(4, 6, empty);
			if((str != english7) || (str.size() != 4) || (str.numCodePointsInString() != 4))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(english9);
			str.replace(2, 4, english7);
			if((str != english12) || (str.size() != 8) || (str.numCodePointsInString() != 8))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(english10);
			str.replace(0, 2, english7);
			if((str != english12) || (str.size() != 8) || (str.numCodePointsInString() != 8))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(english11);
			str.replace(4, 6, english7);
			if((str != english12) || (str.size() != 8) || (str.numCodePointsInString() != 8))
			{
				failed = true;
			}
		}

		{
			fsds::DynamicString str(russian9);
			str.replace(2, 4, empty);
			if((str != russian7) || (str.size() != 4) || (str.numCodePointsInString() != 8))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(russian10);
			str.replace(0, 2, empty);
			if((str != russian7) || (str.size() != 4) || (str.numCodePointsInString() != 8))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(russian11);
			str.replace(4, 6, empty);
			if((str != russian7) || (str.size() != 4) || (str.numCodePointsInString() != 8))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(russian9);
			str.replace(2, 4, russian7);
			if((str != russian12) || (str.size() != 8) || (str.numCodePointsInString() != 16))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(russian10);
			str.replace(0, 2, russian7);
			if((str != russian12) || (str.size() != 8) || (str.numCodePointsInString() != 16))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(russian11);
			str.replace(4, 6, russian7);
			if((str != russian12) || (str.size() != 8) || (str.numCodePointsInString() != 16))
			{
				failed = true;
			}
		}

		{
			fsds::DynamicString str(chinese9);
			str.replace(2, 4, empty);
			if((str != chinese7) || (str.size() != 4) || (str.numCodePointsInString() != 12) )
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(chinese10);
			str.replace(0, 2, empty);
			if((str != chinese7) || (str.size() != 4) || (str.numCodePointsInString() != 12))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(chinese11);
			str.replace(4, 6, empty);
			if((str != chinese7) || (str.size() != 4) || (str.numCodePointsInString() != 12))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(chinese9);
			str.replace(2, 4, chinese7);
			if((str != chinese12) || (str.size() != 8) || (str.numCodePointsInString() != 24))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(chinese10);
			str.replace(0, 2, chinese7);
			if((str != chinese12) || (str.size() != 8) || (str.numCodePointsInString() != 24))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(chinese11);
			str.replace(4, 6, chinese7);
			if((str != chinese12) || (str.size() != 8) || (str.numCodePointsInString() != 24))
			{
				failed = true;
			}
		}

		if(failed)
		{
			testsFailed.push_back(19);
		}
	}

	//test 20: replace old new
	{
		bool failed = false;

		{
			fsds::DynamicString str(english9);
			str.replace(english8, empty);
			if((str != english7) || (str.size() != 4) || (str.numCodePointsInString() != 4))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(russian9);
			str.replace(russian8, empty);
			if((str != russian7) || (str.size() != 4) || (str.numCodePointsInString() != 8))
			{
				failed = true;
			}
		}
		{
			fsds::DynamicString str(chinese9);
			str.replace(chinese8, empty);
			if((str != chinese7) || (str.size() != 4) || (str.numCodePointsInString() != 12))
			{
				failed = true;
			}
		}

		if(failed)
		{
			testsFailed.push_back(20);
		}
	}

	if(testsFailed.size() == 0)
	{
		std::cout << "DynamicString passed all tests" << std::endl;
	}
	else
	{
		if(testsFailed.size() == 1)
		{
			std::cout << "DynamicString failed test " << testsFailed[0] << std::endl;
		}
		else
		{
			std::cout << "DynamicString failed tests ";
			for(size_t i = 0; i < testsFailed.size() - 1; i++)
			{
				std::cout << testsFailed[i] << ", ";
			}
			std::cout << "and " << testsFailed[testsFailed.size()-1] << std::endl;
		}
	}
}

void testInlineList()
{
	std::vector<size_t> testsFailed;
	
	size_t testSize = 100;
	fsds::InlineList<size_t> list;

	//test 1: simple constructor
	if(list.capacity() < 16)
	{
		testsFailed.push_back(1);
	}

	//test 2: append and operator[]
	for(size_t i = 0; i < testSize; i++)
	{
		list.append(i);
	}
	for(size_t i = 0; i < testSize; i++)
	{
		if(list[i] != i)
		{
			testsFailed.push_back(2);
			break;
		}
	}

	//test 3: front
	if(list.front() != 0)
	{
		testsFailed.push_back(3);
	}

	//test 4: back
	if(list.back() != 99)
	{
		testsFailed.push_back(4);
	}

	//test 5: size
	if(list.size() != 100)
	{
		testsFailed.push_back(5);
	}

	//test 6: isEmpty
	if(list.isEmpty())
	{
		testsFailed.push_back(6);
	}

	//test 7: clear
	list.clear();
	if(list.size() != 0)
	{
		testsFailed.push_back(7);
	}

	//test 8: prepend
	for(size_t i = 0; i < testSize; i++)
	{
		list.prepend(i);
	}
	for(size_t i = 0; i < testSize; i++)
	{
		if(list[i] != testSize - i - 1)
		{
			testsFailed.push_back(8);
			break;
		}
	}

	//test 9: insert at the front
	list.insert(0, testSize);
	if(list[0] != testSize)
	{
		testsFailed.push_back(9);
	}

	//test 10: insert in the middle
	list.insert(50, testSize+1);
	if(list[50] != testSize+1)
	{
		testsFailed.push_back(10);
	}

	//test 11: insert at the end
	list.insert(list.size(), testSize+2);
	if(list[list.size()-1] != testSize+2)
	{
		testsFailed.push_back(11);
	}

	//test 12: removeFront
	list.removeFront();
	if(list[0] != testSize-1)
	{
		testsFailed.push_back(12);
	}
	else if(list[list.size()-1] != testSize+2)
	{
		testsFailed.push_back(12);
	}

	//test 13: removeBack
	list.removeBack();
	if(list[list.size()-1] != 0)
	{
		testsFailed.push_back(13);
	}
	else if(list[0] != testSize-1)
	{
		testsFailed.push_back(13);
	}

	//test 14: remove from the front
	list.remove(0);
	if(list[0] != testSize-2)
	{
		testsFailed.push_back(14);
	}
	else if(list[list.size()-1] != 0)
	{
		testsFailed.push_back(14);
	}

	//test 15: remove from the back
	list.remove(list.size()-1);
	if(list[list.size()-1] != 1)
	{
		testsFailed.push_back(15);
	}
	else if(list[0] != testSize-2)
	{
		testsFailed.push_back(15);
	}

	//test 16: remove from the middle
	list.remove(48);
	for(size_t i = 0; i < testSize - 2; i++)
	{
		if(list[i] != testSize - i - 2)
		{
			testsFailed.push_back(16);
			break;
		}
	}

	//test 17: deepCopy
	fsds::InlineList<size_t> listDuplicate;
	list.deepCopy(listDuplicate);

	for(size_t i = 0; i < list.size(); i++)
	{
		if(list[i] != listDuplicate[i])
		{
			testsFailed.push_back(17);
			break;
		}
	}

	//test 18: valueEquality
	if(list.valueEquality(listDuplicate) == false)
	{
		testsFailed.push_back(18);
	}


	if(testsFailed.size() == 0)
	{
		std::cout << "InlineList passed all tests" << std::endl;
	}
	else
	{
		if(testsFailed.size() == 1)
		{
			std::cout << "InlineList failed test " << testsFailed[0] << std::endl;
		}
		else
		{
			std::cout << "InlineList failed tests ";
			for(size_t i = 0; i < testsFailed.size() - 1; i++)
			{
				std::cout << testsFailed[i] << ", ";
			}
			std::cout << "and " << testsFailed[testsFailed.size()-1] << std::endl;
		}
	}
}

void testStableListC()
{
	const size_t elementSize = sizeof(size_t);
	const size_t testMagicNumber = 1000;
	std::vector<size_t> testsFailed;

	FSDS_StableList list;

	int retcode;

	//test 1: constructor
	retcode = FSDS_StableList_constructSizeFront(&list, 20, 5, elementSize);
	FSDS_StableListHeader* header = &list.memBlock->header;
	if(retcode)
	{
		std::cout << "StableList (C) failed test 1. Constructor returned error." << std::endl;
		return;
	}
	if(header->size != 0)
	{
		std::cout << "StableList (C) failed test 1. Incorrect size." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->capacity != 20)
	{
		std::cout << "StableList (C) failed test 1. Incorrect capacity." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->front != 5)
	{
		std::cout << "StableList (C) failed test 1. Incorrect front." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->iRefOffset != header->instanceIRefOffsetStart)
	{
		std::cout << "StableList (C) failed test 1. Incorrect iRefOffset." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->reallocationFrontMargin != 5)
	{
		std::cout << "StableList (C) failed test 1. Incorrect reallocationFrontMargin." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->elementSize != elementSize)
	{
		std::cout << "StableList (C) failed test 1. Incorrect elementSize." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}

	//test 2: destructor
	retcode = FSDS_StableList_destroy(list);
	if(retcode)
	{
		std::cout << "StableList (C) failed test 2. Destructor returned error." << std::endl;
		return;
	}

	//reconstruct the list for future testing knowing the constructor works
	FSDS_StableList_constructSizeFront(&list, 20, 5, elementSize);
	header = &list.memBlock->header;

	//test 3: adding elements with out reallocation
	for(size_t i = 0; i < 10; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_appendBack(&list, &ptr);
		header = &list.memBlock->header;
		if(retcode)
		{
			std::cout << "StableList (C) failed test 3. appendBack returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		*reinterpret_cast<size_t*>(ptr) = i + testMagicNumber;
	}
	if(header->size != 10)
	{
		std::cout << "StableList (C) failed test 3. Incorrect size." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->capacity != 20)
	{
		std::cout << "StableList (C) failed test 3. Incorrect capacity." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->front != 5)
	{
		std::cout << "StableList (C) failed test 3. Incorrect front." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	for(size_t i = 0; i < 10; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIndex(list, i, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 3. getElementIndex returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != i + testMagicNumber)
		{
			std::cout << "StableList (C) failed test 3. Incorrect value at index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}

	//create an iRef tracking index 5 (value 5 + testMagicNumber) for use in tests 4, 5, 6
	FSDS_StableList_IRef trackedIRef;
	retcode = FSDS_StableList_indexToIRef(list, 5, &trackedIRef);
	if(retcode)
	{
		std::cout << "StableList (C) failed to create tracked iRef." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	const size_t iRefExpectedValue = 5 + testMagicNumber;

	//test 4: adding elements with reallocation
	for(size_t i = 10; i < 50; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_appendBack(&list, &ptr);
		header = &list.memBlock->header;
		if(retcode)
		{
			std::cout << "StableList (C) failed test 4. appendBack returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		*reinterpret_cast<size_t*>(ptr) = i + testMagicNumber;
		retcode = FSDS_StableList_getElementIRef(list, trackedIRef, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 4. getElementIRef returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != iRefExpectedValue)
		{
			std::cout << "StableList (C) failed test 4. Tracked iRef points to incorrect value on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	if(header->size != 50)
	{
		std::cout << "StableList (C) failed test 4. Incorrect size." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->capacity != 80)
	{
		std::cout << "StableList (C) failed test 4. Incorrect capacity." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->front != 5)
	{
		std::cout << "StableList (C) failed test 4. incorrect front." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	for(size_t i = 10; i < 50; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIndex(list, i, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 4. getElementIndex returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != i + testMagicNumber)
		{
			std::cout << "StableList (C) failed test 4. Incorrect value at index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}

	//test 5: adding elements to front without reallocation
	for(size_t i = 0; i < 5; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_appendFront(&list, &ptr);
		header = &list.memBlock->header;
		if(retcode)
		{
			std::cout << "StableList (C) failed test 5. appendFront returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		*reinterpret_cast<size_t*>(ptr) = (50 + i) + testMagicNumber;
		retcode = FSDS_StableList_getElementIRef(list, trackedIRef, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 5. getElementIRef returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != iRefExpectedValue)
		{
			std::cout << "StableList (C) failed test 5. Tracked iRef points to incorrect value on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	if(header->size != 55)
	{
		std::cout << "StableList (C) failed test 5. Incorrect size." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->capacity != 80)
	{
		std::cout << "StableList (C) failed test 5. Capacity changed unexpectedly." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->front != 0)
	{
		std::cout << "StableList (C) failed test 5. Incorrect front." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	for(size_t i = 0; i < 5; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIndex(list, i, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 5. getElementIndex returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != (54 - i) + testMagicNumber)
		{
			std::cout << "StableList (C) failed test 5. Incorrect value at index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	for(size_t i = 5; i < 55; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIndex(list, i, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 5. getElementIndex returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != (i - 5) + testMagicNumber)
		{
			std::cout << "StableList (C) failed test 5. Incorrect value at index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}

	//test 6: adding elements to front with reallocation
	for(size_t i = 0; i < 20; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_appendFront(&list, &ptr);
		header = &list.memBlock->header;
		if(retcode)
		{
			std::cout << "StableList (C) failed test 6. appendFront returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		*reinterpret_cast<size_t*>(ptr) = (55 + i) + testMagicNumber;
		retcode = FSDS_StableList_getElementIRef(list, trackedIRef, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 6. getElementIRef returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != iRefExpectedValue)
		{
			std::cout << "StableList (C) failed test 6. Tracked iRef points to incorrect value on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	if(header->size != 75)
	{
		std::cout << "StableList (C) failed test 6. Incorrect size." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->capacity != 100)
	{
		std::cout << "StableList (C) failed test 6. Incorrect capacity." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->front != 0)
	{
		std::cout << "StableList (C) failed test 6. Incorrect front." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	for(size_t i = 0; i < 20; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIndex(list, i, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 6. getElementIndex returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != (74 - i) + testMagicNumber)
		{
			std::cout << "StableList (C) failed test 6. Incorrect value at index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	for(size_t i = 20; i < 25; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIndex(list, i, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 6. getElementIndex returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != (74 - i) + testMagicNumber)
		{
			std::cout << "StableList (C) failed test 6. Incorrect value at index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	for(size_t i = 25; i < 75; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIndex(list, i, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 6. getElementIndex returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != (i - 25) + testMagicNumber)
		{
			std::cout << "StableList (C) failed test 6. Incorrect value at index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}

	//test 7: removing elements from front and back, checking iRef each time
	for(size_t i = 0; i < 25; i++)
	{
		retcode = FSDS_StableList_removeFront(list);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 7. removeFront returned error on iteration " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		void* ptr;
		retcode = FSDS_StableList_getElementIRef(list, trackedIRef, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 7. getElementIRef returned error after removeFront iteration " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != iRefExpectedValue)
		{
			std::cout << "StableList (C) failed test 7. Tracked iRef points to incorrect value after removeFront iteration " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(header->capacity != 100)
		{
			std::cout << "StableList (C) failed test 7. Incorrect capacity after removeFront iteration " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(header->front != i + 1)
		{
			std::cout << "StableList (C) failed test 7. Incorrect front after removeFront iteration " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	for(size_t i = 0; i < 10; i++)
	{
		retcode = FSDS_StableList_removeBack(list);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 7. removeBack returned error on iteration " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		void* ptr;
		retcode = FSDS_StableList_getElementIRef(list, trackedIRef, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 7. getElementIRef returned error after removeBack iteration " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != iRefExpectedValue)
		{
			std::cout << "StableList (C) failed test 7. Tracked iRef points to incorrect value after removeBack iteration " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(header->capacity != 100)
		{
			std::cout << "StableList (C) failed test 7. Incorrect capacity after removeBack iteration " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(header->front != 25)
		{
			std::cout << "StableList (C) failed test 7. Incorrect front after removeBack iteration " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	if(header->size != 40)
	{
		std::cout << "StableList (C) failed test 7. Incorrect size. Expected 40, got " << header->size << "." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}

	//test 8: appendBack to 120 elements, triggering reallocation, check iRef each time
	for(size_t i = 0; i < 80; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_appendBack(&list, &ptr);
		header = &list.memBlock->header;
		if(retcode)
		{
			std::cout << "StableList (C) failed test 8. appendBack returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		*reinterpret_cast<size_t*>(ptr) = (40 + i) + testMagicNumber;
		retcode = FSDS_StableList_getElementIRef(list, trackedIRef, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 8. getElementIRef returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != iRefExpectedValue)
		{
			std::cout << "StableList (C) failed test 8. Tracked iRef points to incorrect value on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	if(header->size != 120)
	{
		std::cout << "StableList (C) failed test 8. Incorrect size. Expected 120, got " << header->size << "." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}

	//test 9: indexToIRef for all elements, verify via getElementIRef
	for(size_t i = 0; i < 120; i++)
	{
		FSDS_StableList_IRef iRef;
		retcode = FSDS_StableList_indexToIRef(list, i, &iRef);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 9. indexToIRef returned error on index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		void* ptr;
		retcode = FSDS_StableList_getElementIRef(list, iRef, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 9. getElementIRef returned error on index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != i + testMagicNumber)
		{
			std::cout << "StableList (C) failed test 9. Incorrect value at index " << i << ". Expected " << i + testMagicNumber << ", got " << *reinterpret_cast<size_t*>(ptr) << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	//check that indexToIRef returns an error for an out-of-bounds index
	{
		FSDS_StableList_IRef iRef;
		retcode = FSDS_StableList_indexToIRef(list, 120, &iRef);
		if(!retcode)
		{
			std::cout << "StableList (C) failed test 9. indexToIRef did not return error for out-of-bounds index." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}

	//test 10: iRefToIndex for all elements, verify round-trip with indexToIRef
	for(size_t i = 0; i < 120; i++)
	{
		FSDS_StableList_IRef iRef;
		retcode = FSDS_StableList_indexToIRef(list, i, &iRef);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 10. indexToIRef returned error on index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		size_t roundTrippedIndex;
		retcode = FSDS_StableList_iRefToIndex(list, iRef, &roundTrippedIndex);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 10. iRefToIndex returned error on index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(roundTrippedIndex != i)
		{
			std::cout << "StableList (C) failed test 10. Round-trip failed on index " << i << ". Expected " << i << ", got " << roundTrippedIndex << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}

	//test 11: reserve to a larger capacity, check size and iRef preserved
	retcode = FSDS_StableList_reserve(&list, 500);
	header = &list.memBlock->header;
	if(retcode)
	{
		std::cout << "StableList (C) failed test 11. reserve returned error." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->size != 120)
	{
		std::cout << "StableList (C) failed test 11. Incorrect size. Expected 120, got " << header->size << "." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->capacity != 500)
	{
		std::cout << "StableList (C) failed test 11. Incorrect capacity. Expected 500, got " << header->capacity << "." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIRef(list, trackedIRef, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 11. getElementIRef returned error." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != iRefExpectedValue)
		{
			std::cout << "StableList (C) failed test 11. Tracked iRef points to incorrect value." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	//check that reserving smaller than size returns an error
	retcode = FSDS_StableList_reserve(&list, 50);
	header = &list.memBlock->header;
	if(!retcode)
	{
		std::cout << "StableList (C) failed test 11. reserve did not return error for capacity smaller than size." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	//verify values are intact
	for(size_t i = 0; i < 120; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIndex(list, i, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 11. getElementIndex returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != i + testMagicNumber)
		{
			std::cout << "StableList (C) failed test 11. Incorrect value at index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}

	//test 12: shrink_to_fit, check capacity equals size, front becomes 0, iRef preserved
	retcode = FSDS_StableList_shrinkToFit(&list);
	header = &list.memBlock->header;
	if(retcode)
	{
		std::cout << "StableList (C) failed test 12. shrink_to_fit returned error." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->size != 120)
	{
		std::cout << "StableList (C) failed test 12. Incorrect size. Expected 120, got " << header->size << "." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->capacity != 120)
	{
		std::cout << "StableList (C) failed test 12. Incorrect capacity. Expected 120, got " << header->capacity << "." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->front != 0)
	{
		std::cout << "StableList (C) failed test 12. Incorrect front. Expected 0, got " << header->front << "." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIRef(list, trackedIRef, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 12. getElementIRef returned error." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != iRefExpectedValue)
		{
			std::cout << "StableList (C) failed test 12. Tracked iRef points to incorrect value." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}
	//verify values are intact
	for(size_t i = 0; i < 120; i++)
	{
		void* ptr;
		retcode = FSDS_StableList_getElementIndex(list, i, &ptr);
		if(retcode)
		{
			std::cout << "StableList (C) failed test 12. getElementIndex returned error on element " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(*reinterpret_cast<size_t*>(ptr) != i + testMagicNumber)
		{
			std::cout << "StableList (C) failed test 12. Incorrect value at index " << i << "." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}

	//test 13: clear, check size becomes 0, capacity unchanged
	retcode = FSDS_StableList_clear(list);
	if(retcode)
	{
		std::cout << "StableList (C) failed test 13. clear returned error." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->size != 0)
	{
		std::cout << "StableList (C) failed test 13. Incorrect size. Expected 0, got " << header->size << "." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	if(header->capacity != 120)
	{
		std::cout << "StableList (C) failed test 13. Capacity changed unexpectedly. Expected 120, got " << header->capacity << "." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}

	//test 14: check iRef errors correct idenfity wrong instance
	FSDS_StableList_IRef underflowIRef(0);
	FSDS_StableList_IRef overflowIRef(0x1000000000000);
	{
		size_t index;
		retcode = FSDS_StableList_iRefToIndex(list, underflowIRef, &index);
		if(retcode != FSDS_STABLE_LIST_ERROR_IREF_DIFFERENT_INSTANCE)
		{
			std::cout << "StableList (C) failed test 14. Did not identify iRef came from different iRef." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		retcode = FSDS_StableList_iRefToIndex(list, overflowIRef, &index);
		if(retcode != FSDS_STABLE_LIST_ERROR_IREF_DIFFERENT_INSTANCE)
		{
			std::cout << "StableList (C) failed test 14. Did not identify iRef came from different iRef." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}

	//test 15: check that FSDS_STABLE_LIST_INSTANCE_IREF_OFFSET_INCREMENT is calculated correctly
	if(FSDS_STABLE_LIST_INSTANCE_IREF_OFFSET_INCREMENT == 0)
	{
		std::cout << "StableList (C) failed test 15. FSDS_STABLE_LIST_INSTANCE_IREF_OFFSET_INCREMENT is zero." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}
	{
		size_t popCount = 0;
		size_t n = FSDS_STABLE_LIST_INSTANCE_IREF_OFFSET_INCREMENT;
		for(int i = 0; i < CHAR_BIT * sizeof(void*); i++)
		{
			popCount += n & 0b1;
			n >>= 1;
		}
		if(popCount != 1)
		{
			std::cout << "StableList (C) failed test 15. FSDS_STABLE_LIST_INSTANCE_IREF_OFFSET_INCREMENT does not contain a single bit." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
		if(FSDS_STABLE_LIST_INSTANCE_IREF_OFFSET_INCREMENT * FSDS_STABLE_LIST_INSTANCE_IREF_OFFSET_INCREMENT != 0)
		{
			std::cout << "StableList (C) failed test 15. FSDS_STABLE_LIST_INSTANCE_IREF_OFFSET_INCREMENT does not square to zero (with overflow)." << std::endl;
			FSDS_StableList_destroy(list);
			return;
		}
	}

	//test 16: offset of data in memory block
	if(offsetof(FSDS_StableListBlock, data) != FSDS_STABLE_LIST_CACHE_LINE_SIZE)
	{
		std::cout << "StableList (C) failed test 16. Data member of FSDS_StableListBlock is not alligned to a cache line." << std::endl;
		FSDS_StableList_destroy(list);
		return;
	}

	FSDS_StableList_destroy(list);
	std::cout << "StableList (C) passed all tests" << std::endl;
}

template class fsds::StableList<size_t>;

void testStableListCpp()
{
	const size_t testMagicNumber = 2000;
	std::vector<size_t> testsFailed;

	//test 1: default constructor
	{
		fsds::StableList<size_t> list;
		if(list.size() != 0)
		{
			std::cout << "StableList (Cpp) failed test 1. Incorrect size. Expected 0, got " << list.size() << "." << std::endl;
			return;
		}
		if(!list.isEmpty())
		{
			std::cout << "StableList (Cpp) failed test 1. isEmpty() returned false on default-constructed list." << std::endl;
			return;
		}
	}

	//test 2: size constructor
	{
		fsds::StableList<size_t> list(10);
		if(list.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 2. Incorrect size. Expected 10, got " << list.size() << "." << std::endl;
			return;
		}
	}

	//test 3: fill constructor
	{
		fsds::StableList<size_t> list(10, testMagicNumber);
		if(list.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 3. Incorrect size. Expected 10, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(list.at(i) != testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 3. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 4: copy constructor
	{
		fsds::StableList<size_t> src(10, testMagicNumber);
		fsds::StableList<size_t> dst(src);
		if(dst.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 4. Incorrect size in copy. Expected 10, got " << dst.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(dst.at(i) != testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 4. Incorrect value at index " << i << " in copy." << std::endl;
				return;
			}
		}
		dst.append(testMagicNumber + 1);
		if(src.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 4. Modifying copy affected source." << std::endl;
			return;
		}
	}

	//test 5: move constructor
	{
		fsds::StableList<size_t> src(10, testMagicNumber);
		fsds::StableList<size_t> dst(std::move(src));
		if(dst.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 5. Incorrect size after move. Expected 10, got " << dst.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(dst.at(i) != testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 5. Incorrect value at index " << i << " after move." << std::endl;
				return;
			}
		}
	}

	//test 6: iterator constructor
	{
		std::vector<size_t> src;
		for(size_t i = 0; i < 10; i++) { src.push_back(i + testMagicNumber); }
		fsds::StableList<size_t> list(src.begin(), src.end());
		if(list.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 6. Incorrect size. Expected 10, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 6. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 7: initializer list constructor
	{
		fsds::StableList<size_t> list = {100, 200, 300, 400, 500};
		const size_t expected[] = {100, 200, 300, 400, 500};
		if(list.size() != 5)
		{
			std::cout << "StableList (Cpp) failed test 7. Incorrect size. Expected 5, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 5; i++)
		{
			if(list.at(i) != expected[i])
			{
				std::cout << "StableList (Cpp) failed test 7. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 8: copy assign
	{
		fsds::StableList<size_t> src(10, testMagicNumber);
		fsds::StableList<size_t> dst(3, testMagicNumber + 1);
		dst = src;
		if(dst.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 8. Incorrect size after copy assign. Expected 10, got " << dst.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(dst.at(i) != testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 8. Incorrect value at index " << i << " after copy assign." << std::endl;
				return;
			}
		}
		dst.append(testMagicNumber + 2);
		if(src.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 8. Modifying copy affected source." << std::endl;
			return;
		}
	}

	//test 9: move assign
	{
		fsds::StableList<size_t> src(10, testMagicNumber);
		fsds::StableList<size_t> dst(3, testMagicNumber + 1);
		dst = std::move(src);
		if(dst.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 9. Incorrect size after move assign. Expected 10, got " << dst.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(dst.at(i) != testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 9. Incorrect value at index " << i << " after move assign." << std::endl;
				return;
			}
		}
	}

	//test 10: initializer list assign (operator=)
	{
		fsds::StableList<size_t> list(3, testMagicNumber);
		list = {100, 200, 300, 400, 500};
		const size_t expected[] = {100, 200, 300, 400, 500};
		if(list.size() != 5)
		{
			std::cout << "StableList (Cpp) failed test 10. Incorrect size. Expected 5, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 5; i++)
		{
			if(list.at(i) != expected[i])
			{
				std::cout << "StableList (Cpp) failed test 10. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 11: fill assign
	{
		fsds::StableList<size_t> list(3, testMagicNumber + 1);
		list.assign(10, testMagicNumber);
		if(list.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 11. Incorrect size. Expected 10, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(list.at(i) != testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 11. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 12: iterator assign
	{
		std::vector<size_t> src;
		for(size_t i = 0; i < 10; i++) { src.push_back(i + testMagicNumber); }
		fsds::StableList<size_t> list(3, testMagicNumber + 1);
		list.assign(src.begin(), src.end());
		if(list.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 12. Incorrect size. Expected 10, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 12. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 13: initializer list assign (method)
	{
		fsds::StableList<size_t> list(3, testMagicNumber);
		list.assign({100, 200, 300, 400, 500});
		const size_t expected[] = {100, 200, 300, 400, 500};
		if(list.size() != 5)
		{
			std::cout << "StableList (Cpp) failed test 13. Incorrect size. Expected 5, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 5; i++)
		{
			if(list.at(i) != expected[i])
			{
				std::cout << "StableList (Cpp) failed test 13. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 14: operator[](size_t) read, write, and const overload
	{
		fsds::StableList<size_t> list(10, testMagicNumber);
		for(size_t i = 0; i < 10; i++)
		{
			if(list[i] != testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 14. Non-const operator[](size_t) returned incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
		for(size_t i = 0; i < 10; i++)
		{
			list[i] = i + testMagicNumber;
		}
		const fsds::StableList<size_t>& clist = list;
		for(size_t i = 0; i < 10; i++)
		{
			if(clist[i] != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 14. Const operator[](size_t) returned incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 15: operator[](IRef) read, write, and const overload
	{
		fsds::StableList<size_t> list(10, testMagicNumber);
		for(size_t i = 0; i < 10; i++)
		{
			typename fsds::StableList<size_t>::IRef ref = list.indexToIRef(i);
			if(list[ref] != testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 15. Non-const operator[](IRef) returned incorrect value at index " << i << "." << std::endl;
				return;
			}
			list[ref] = i + testMagicNumber;
		}
		const fsds::StableList<size_t>& clist = list;
		for(size_t i = 0; i < 10; i++)
		{
			typename fsds::StableList<size_t>::IRef ref = list.indexToIRef(i);
			if(clist[ref] != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 15. Const operator[](IRef) returned incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 16: at(size_t) read, write, and const overload
	{
		fsds::StableList<size_t> list(10, testMagicNumber);
		for(size_t i = 0; i < 10; i++)
		{
			if(list.at(i) != testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 16. Non-const at(size_t) returned incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
		for(size_t i = 0; i < 10; i++)
		{
			list.at(i) = i + testMagicNumber;
		}
		const fsds::StableList<size_t>& clist = list;
		for(size_t i = 0; i < 10; i++)
		{
			if(clist.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 16. Const at(size_t) returned incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 17: at(IRef) read, write, and const overload
	{
		fsds::StableList<size_t> list(10, testMagicNumber);
		for(size_t i = 0; i < 10; i++)
		{
			typename fsds::StableList<size_t>::IRef ref = list.indexToIRef(i);
			if(list.at(ref) != testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 17. Non-const at(IRef) returned incorrect value at index " << i << "." << std::endl;
				return;
			}
			list.at(ref) = i + testMagicNumber;
		}
		const fsds::StableList<size_t>& clist = list;
		for(size_t i = 0; i < 10; i++)
		{
			typename fsds::StableList<size_t>::IRef ref = list.indexToIRef(i);
			if(clist.at(ref) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 17. Const at(IRef) returned incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 18: front, front() const, and frontIRef
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 10; i++) { list.append(i + testMagicNumber); }
		if(list.front() != testMagicNumber)
		{
			std::cout << "StableList (Cpp) failed test 18. Non-const front() returned incorrect value." << std::endl;
			return;
		}
		list.front() = testMagicNumber + 100;
		const fsds::StableList<size_t>& clist = list;
		if(clist.front() != testMagicNumber + 100)
		{
			std::cout << "StableList (Cpp) failed test 18. Const front() did not reflect write." << std::endl;
			return;
		}
		typename fsds::StableList<size_t>::IRef ref = list.frontIRef();
		if(list.at(ref) != testMagicNumber + 100)
		{
			std::cout << "StableList (Cpp) failed test 18. frontIRef() does not resolve to the front element." << std::endl;
			return;
		}
		if(list.iRefToIndex(ref) != 0)
		{
			std::cout << "StableList (Cpp) failed test 18. frontIRef() does not resolve to index 0." << std::endl;
			return;
		}
	}

	//test 19: back, back() const, and backIRef
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 10; i++) { list.append(i + testMagicNumber); }
		if(list.back() != 9 + testMagicNumber)
		{
			std::cout << "StableList (Cpp) failed test 19. Non-const back() returned incorrect value." << std::endl;
			return;
		}
		list.back() = testMagicNumber + 100;
		const fsds::StableList<size_t>& clist = list;
		if(clist.back() != testMagicNumber + 100)
		{
			std::cout << "StableList (Cpp) failed test 19. Const back() did not reflect write." << std::endl;
			return;
		}
		typename fsds::StableList<size_t>::IRef ref = list.backIRef();
		if(list.at(ref) != testMagicNumber + 100)
		{
			std::cout << "StableList (Cpp) failed test 19. backIRef() does not resolve to the back element." << std::endl;
			return;
		}
		if(list.iRefToIndex(ref) != list.size() - 1)
		{
			std::cout << "StableList (Cpp) failed test 19. backIRef() does not resolve to the last index." << std::endl;
			return;
		}
	}

	//test 20: empty and isEmpty
	{
		fsds::StableList<size_t> list;
		if(!list.empty())
		{
			std::cout << "StableList (Cpp) failed test 20. empty() returned false on fresh list." << std::endl;
			return;
		}
		if(!list.isEmpty())
		{
			std::cout << "StableList (Cpp) failed test 20. isEmpty() returned false on fresh list." << std::endl;
			return;
		}
		list.append(testMagicNumber);
		if(list.empty())
		{
			std::cout << "StableList (Cpp) failed test 20. empty() returned true after append." << std::endl;
			return;
		}
		if(list.isEmpty())
		{
			std::cout << "StableList (Cpp) failed test 20. isEmpty() returned true after append." << std::endl;
			return;
		}
	}

	//test 21: size
	{
		fsds::StableList<size_t> list;
		if(list.size() != 0)
		{
			std::cout << "StableList (Cpp) failed test 21. size() was " << list.size() << ", expected 0 on fresh list." << std::endl;
			return;
		}
		for(size_t i = 0; i < 25; i++) { list.append(i + testMagicNumber); }
		if(list.size() != 25)
		{
			std::cout << "StableList (Cpp) failed test 21. size() was " << list.size() << ", expected 25 after 25 appends." << std::endl;
			return;
		}
	}

	//test 22: maxSize
	{
		fsds::StableList<size_t> list;
		if(list.maxSize() != std::numeric_limits<size_t>::max())
		{
			std::cout << "StableList (Cpp) failed test 22. maxSize() did not equal numeric_limits<size_t>::max()." << std::endl;
			return;
		}
	}

	//test 23: capacity grows with size
	{
		fsds::StableList<size_t> list;
		const size_t initialCapacity = list.capacity();
		if(initialCapacity == 0)
		{
			std::cout << "StableList (Cpp) failed test 23. Initial capacity was 0." << std::endl;
			return;
		}
		for(size_t i = 0; i < initialCapacity * 4; i++) { list.append(i + testMagicNumber); }
		if(list.capacity() < list.size())
		{
			std::cout << "StableList (Cpp) failed test 23. capacity() < size() after appends." << std::endl;
			return;
		}
		if(list.capacity() <= initialCapacity)
		{
			std::cout << "StableList (Cpp) failed test 23. capacity() did not grow after exceeding initial capacity." << std::endl;
			return;
		}
	}

	//test 24: reserve
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 10; i++) { list.append(i + testMagicNumber); }
		list.reserve(500);
		if(list.capacity() < 500)
		{
			std::cout << "StableList (Cpp) failed test 24. capacity() was " << list.capacity() << ", expected at least 500 after reserve(500)." << std::endl;
			return;
		}
		if(list.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 24. size() changed after reserve. Expected 10, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 24. Value at index " << i << " changed after reserve." << std::endl;
				return;
			}
		}
	}

	//test 25: shrinkToFit
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 10; i++) { list.append(i + testMagicNumber); }
		list.reserve(500);
		list.shrinkToFit();
		if(list.capacity() != list.size())
		{
			std::cout << "StableList (Cpp) failed test 25. capacity() was " << list.capacity() << ", expected " << list.size() << " after shrinkToFit." << std::endl;
			return;
		}
		if(list.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 25. size() changed after shrinkToFit. Expected 10, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 25. Value at index " << i << " changed after shrinkToFit." << std::endl;
				return;
			}
		}
	}

	//test 26: shrink_to_fit
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 10; i++) { list.append(i + testMagicNumber); }
		list.reserve(500);
		list.shrink_to_fit();
		if(list.capacity() != list.size())
		{
			std::cout << "StableList (Cpp) failed test 26. capacity() was " << list.capacity() << ", expected " << list.size() << " after shrink_to_fit." << std::endl;
			return;
		}
		if(list.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 26. size() changed after shrink_to_fit. Expected 10, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 26. Value at index " << i << " changed after shrink_to_fit." << std::endl;
				return;
			}
		}
	}

	//test 27: clear
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 10; i++) { list.append(i + testMagicNumber); }
		const size_t preClearCapacity = list.capacity();
		list.clear();
		if(list.size() != 0)
		{
			std::cout << "StableList (Cpp) failed test 27. size() was " << list.size() << ", expected 0 after clear." << std::endl;
			return;
		}
		if(!list.isEmpty())
		{
			std::cout << "StableList (Cpp) failed test 27. isEmpty() returned false after clear." << std::endl;
			return;
		}
		if(list.capacity() != preClearCapacity)
		{
			std::cout << "StableList (Cpp) failed test 27. capacity() changed after clear. Expected " << preClearCapacity << ", got " << list.capacity() << "." << std::endl;
			return;
		}
		list.append(testMagicNumber);
		if(list.size() != 1 || list.at(0) != testMagicNumber)
		{
			std::cout << "StableList (Cpp) failed test 27. List unusable after clear." << std::endl;
			return;
		}
	}

	//test 28: append
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.append(i + testMagicNumber); }
		if(list.size() != 50)
		{
			std::cout << "StableList (Cpp) failed test 28. Incorrect size. Expected 50, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 50; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 28. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 29: prepend
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.prepend(i + testMagicNumber); }
		if(list.size() != 50)
		{
			std::cout << "StableList (Cpp) failed test 29. Incorrect size. Expected 50, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 50; i++)
		{
			const size_t expected = (49 - i) + testMagicNumber;
			if(list.at(i) != expected)
			{
				std::cout << "StableList (Cpp) failed test 29. Incorrect value at index " << i << ". Expected " << expected << ", got " << list.at(i) << "." << std::endl;
				return;
			}
		}
	}

	//test 30: appendConstruct
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.appendConstruct(i + testMagicNumber); }
		if(list.size() != 50)
		{
			std::cout << "StableList (Cpp) failed test 30. Incorrect size. Expected 50, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 50; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 30. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 31: prependConstruct
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.prependConstruct(i + testMagicNumber); }
		if(list.size() != 50)
		{
			std::cout << "StableList (Cpp) failed test 31. Incorrect size. Expected 50, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 50; i++)
		{
			const size_t expected = (49 - i) + testMagicNumber;
			if(list.at(i) != expected)
			{
				std::cout << "StableList (Cpp) failed test 31. Incorrect value at index " << i << ". Expected " << expected << ", got " << list.at(i) << "." << std::endl;
				return;
			}
		}
	}

	//test 32: push_back
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.push_back(i + testMagicNumber); }
		if(list.size() != 50)
		{
			std::cout << "StableList (Cpp) failed test 32. Incorrect size. Expected 50, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 50; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 32. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 33: push_front
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.push_front(i + testMagicNumber); }
		if(list.size() != 50)
		{
			std::cout << "StableList (Cpp) failed test 33. Incorrect size. Expected 50, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 50; i++)
		{
			const size_t expected = (49 - i) + testMagicNumber;
			if(list.at(i) != expected)
			{
				std::cout << "StableList (Cpp) failed test 33. Incorrect value at index " << i << ". Expected " << expected << ", got " << list.at(i) << "." << std::endl;
				return;
			}
		}
	}

	//test 34: emplace_back
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.emplace_back(i + testMagicNumber); }
		if(list.size() != 50)
		{
			std::cout << "StableList (Cpp) failed test 34. Incorrect size. Expected 50, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 50; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 34. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 35: emplace_front
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.emplace_front(i + testMagicNumber); }
		if(list.size() != 50)
		{
			std::cout << "StableList (Cpp) failed test 35. Incorrect size. Expected 50, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 50; i++)
		{
			const size_t expected = (49 - i) + testMagicNumber;
			if(list.at(i) != expected)
			{
				std::cout << "StableList (Cpp) failed test 35. Incorrect value at index " << i << ". Expected " << expected << ", got " << list.at(i) << "." << std::endl;
				return;
			}
		}
	}

	//test 36: removeBack
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.append(i + testMagicNumber); }
		for(size_t i = 0; i < 20; i++) { list.removeBack(); }
		if(list.size() != 30)
		{
			std::cout << "StableList (Cpp) failed test 36. Incorrect size after removeBack. Expected 30, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 30; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 36. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
		if(list.back() != 29 + testMagicNumber)
		{
			std::cout << "StableList (Cpp) failed test 36. Incorrect back() value after removeBack." << std::endl;
			return;
		}
	}

	//test 37: removeFront
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.append(i + testMagicNumber); }
		for(size_t i = 0; i < 20; i++) { list.removeFront(); }
		if(list.size() != 30)
		{
			std::cout << "StableList (Cpp) failed test 37. Incorrect size after removeFront. Expected 30, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 30; i++)
		{
			const size_t expected = (i + 20) + testMagicNumber;
			if(list.at(i) != expected)
			{
				std::cout << "StableList (Cpp) failed test 37. Incorrect value at index " << i << ". Expected " << expected << ", got " << list.at(i) << "." << std::endl;
				return;
			}
		}
		if(list.front() != 20 + testMagicNumber)
		{
			std::cout << "StableList (Cpp) failed test 37. Incorrect front() value after removeFront." << std::endl;
			return;
		}
	}

	//test 38: removeBackWithoutDeconstruct
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.append(i + testMagicNumber); }
		for(size_t i = 0; i < 20; i++) { list.removeBackWithoutDeconstruct(); }
		if(list.size() != 30)
		{
			std::cout << "StableList (Cpp) failed test 38. Incorrect size after removeBackWithoutDeconstruct. Expected 30, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 30; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 38. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
	}

	//test 39: removeFrontWithoutDeconstruct
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.append(i + testMagicNumber); }
		for(size_t i = 0; i < 20; i++) { list.removeFrontWithoutDeconstruct(); }
		if(list.size() != 30)
		{
			std::cout << "StableList (Cpp) failed test 39. Incorrect size after removeFrontWithoutDeconstruct. Expected 30, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 30; i++)
		{
			const size_t expected = (i + 20) + testMagicNumber;
			if(list.at(i) != expected)
			{
				std::cout << "StableList (Cpp) failed test 39. Incorrect value at index " << i << ". Expected " << expected << ", got " << list.at(i) << "." << std::endl;
				return;
			}
		}
	}

	//test 40: pop_back
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.append(i + testMagicNumber); }
		for(size_t i = 0; i < 20; i++) { list.pop_back(); }
		if(list.size() != 30)
		{
			std::cout << "StableList (Cpp) failed test 40. Incorrect size after pop_back. Expected 30, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 30; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 40. Incorrect value at index " << i << "." << std::endl;
				return;
			}
		}
		if(list.back() != 29 + testMagicNumber)
		{
			std::cout << "StableList (Cpp) failed test 40. Incorrect back() value after pop_back." << std::endl;
			return;
		}
	}

	//test 41: pop_front
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 50; i++) { list.append(i + testMagicNumber); }
		for(size_t i = 0; i < 20; i++) { list.pop_front(); }
		if(list.size() != 30)
		{
			std::cout << "StableList (Cpp) failed test 41. Incorrect size after pop_front. Expected 30, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 30; i++)
		{
			const size_t expected = (i + 20) + testMagicNumber;
			if(list.at(i) != expected)
			{
				std::cout << "StableList (Cpp) failed test 41. Incorrect value at index " << i << ". Expected " << expected << ", got " << list.at(i) << "." << std::endl;
				return;
			}
		}
		if(list.front() != 20 + testMagicNumber)
		{
			std::cout << "StableList (Cpp) failed test 41. Incorrect front() value after pop_front." << std::endl;
			return;
		}
	}

	//test 42: remove all elements then re-add
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 10; i++) { list.append(i + testMagicNumber); }
		for(size_t i = 0; i < 10; i++) { list.removeBack(); }
		if(list.size() != 0 || !list.isEmpty())
		{
			std::cout << "StableList (Cpp) failed test 42. List not empty after removing all elements." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++) { list.append(i + testMagicNumber); }
		if(list.size() != 10)
		{
			std::cout << "StableList (Cpp) failed test 42. Incorrect size after re-adding. Expected 10, got " << list.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 10; i++)
		{
			if(list.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 42. Incorrect value at index " << i << " after re-adding." << std::endl;
				return;
			}
		}
	}

	//test 43: concatenateBack
	{
		fsds::StableList<size_t> a;
		for(size_t i = 0; i < 15; i++) { a.append(i + testMagicNumber); }
		fsds::StableList<size_t> b;
		for(size_t i = 0; i < 20; i++) { b.append(i + testMagicNumber + 100); }
		a.concatenateBack(b);
		if(a.size() != 35)
		{
			std::cout << "StableList (Cpp) failed test 43. Incorrect size after concatenateBack. Expected 35, got " << a.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 15; i++)
		{
			if(a.at(i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 43. Incorrect value at index " << i << " in original range." << std::endl;
				return;
			}
		}
		for(size_t i = 0; i < 20; i++)
		{
			const size_t expected = i + testMagicNumber + 100;
			if(a.at(15 + i) != expected)
			{
				std::cout << "StableList (Cpp) failed test 43. Incorrect value at index " << 15 + i << " in appended range. Expected " << expected << ", got " << a.at(15 + i) << "." << std::endl;
				return;
			}
		}
		if(b.size() != 20)
		{
			std::cout << "StableList (Cpp) failed test 43. Source size changed after concatenateBack. Expected 20, got " << b.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 20; i++)
		{
			if(b.at(i) != i + testMagicNumber + 100)
			{
				std::cout << "StableList (Cpp) failed test 43. Source value at index " << i << " changed after concatenateBack." << std::endl;
				return;
			}
		}
		b.append(testMagicNumber + 999);
		if(a.size() != 35 || a.at(34) != 19 + testMagicNumber + 100)
		{
			std::cout << "StableList (Cpp) failed test 43. Modifying source affected destination (not a deep copy)." << std::endl;
			return;
		}
	}

	//test 44: concatenateFront
	{
		fsds::StableList<size_t> a;
		for(size_t i = 0; i < 15; i++) { a.append(i + testMagicNumber); }
		fsds::StableList<size_t> b;
		for(size_t i = 0; i < 20; i++) { b.append(i + testMagicNumber + 100); }
		a.concatenateFront(b);
		if(a.size() != 35)
		{
			std::cout << "StableList (Cpp) failed test 44. Incorrect size after concatenateFront. Expected 35, got " << a.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 20; i++)
		{
			const size_t expected = i + testMagicNumber + 100;
			if(a.at(i) != expected)
			{
				std::cout << "StableList (Cpp) failed test 44. Incorrect value at index " << i << " in prepended range. Expected " << expected << ", got " << a.at(i) << "." << std::endl;
				return;
			}
		}
		for(size_t i = 0; i < 15; i++)
		{
			if(a.at(20 + i) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 44. Incorrect value at index " << 20 + i << " in original range." << std::endl;
				return;
			}
		}
		if(b.size() != 20)
		{
			std::cout << "StableList (Cpp) failed test 44. Source size changed after concatenateFront. Expected 20, got " << b.size() << "." << std::endl;
			return;
		}
		for(size_t i = 0; i < 20; i++)
		{
			if(b.at(i) != i + testMagicNumber + 100)
			{
				std::cout << "StableList (Cpp) failed test 44. Source value at index " << i << " changed after concatenateFront." << std::endl;
				return;
			}
		}
		b.append(testMagicNumber + 999);
		if(a.size() != 35 || a.at(0) != testMagicNumber + 100)
		{
			std::cout << "StableList (Cpp) failed test 44. Modifying source affected destination (not a deep copy)." << std::endl;
			return;
		}
	}

	//test 45: indexToIRef
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		for(size_t i = 0; i < 20; i++)
		{
			typename fsds::StableList<size_t>::IRef iRef = list.indexToIRef(i);
			if(list.at(iRef) != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 45. indexToIRef returned iRef with incorrect value at index " << i << ". Expected " << i + testMagicNumber << ", got " << list.at(iRef) << "." << std::endl;
				return;
			}
		}
		for(size_t i = 0; i < 20; i++)
		{
			typename fsds::StableList<size_t>::IRef a = list.indexToIRef(i);
			typename fsds::StableList<size_t>::IRef b = list.indexToIRef(i);
			if(a != b)
			{
				std::cout << "StableList (Cpp) failed test 45. indexToIRef not deterministic at index " << i << "." << std::endl;
				return;
			}
		}
		bool threw = false;
		try
		{
			list.indexToIRef(20);
		}
		catch(const std::runtime_error&)
		{
			threw = true;
		}
		if(!threw)
		{
			std::cout << "StableList (Cpp) failed test 45. indexToIRef did not throw for out-of-bounds index." << std::endl;
			return;
		}
	}

	//test 46: iRefToIndex round-trip with indexToIRef
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		for(size_t i = 0; i < 20; i++)
		{
			typename fsds::StableList<size_t>::IRef iRef = list.indexToIRef(i);
			size_t roundTrippedIndex = list.iRefToIndex(iRef);
			if(roundTrippedIndex != i)
			{
				std::cout << "StableList (Cpp) failed test 46. iRefToIndex round-trip failed at index " << i << ". Expected " << i << ", got " << roundTrippedIndex << "." << std::endl;
				return;
			}
		}
		bool threwUnderflow = false;
		try
		{
			typename fsds::StableList<size_t>::IRef bad(0);
			list.iRefToIndex(bad);
		}
		catch(const std::runtime_error&)
		{
			threwUnderflow = true;
		}
		if(!threwUnderflow)
		{
			std::cout << "StableList (Cpp) failed test 46. iRefToIndex did not throw for invalid (underflow) iRef." << std::endl;
			return;
		}
		bool threwOverflow = false;
		try
		{
			typename fsds::StableList<size_t>::IRef bad(0x1000000000000);
			list.iRefToIndex(bad);
		}
		catch(const std::runtime_error&)
		{
			threwOverflow = true;
		}
		if(!threwOverflow)
		{
			std::cout << "StableList (Cpp) failed test 46. iRefToIndex did not throw for invalid (overflow) iRef." << std::endl;
			return;
		}
	}

	//test 47: begin/end forward iteration
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		size_t i = 0;
		for(auto it = list.begin(); it != list.end(); ++it)
		{
			if(*it != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 47. Incorrect value at iterator position " << i << ". Expected " << i + testMagicNumber << ", got " << *it << "." << std::endl;
				return;
			}
			i++;
		}
		if(i != 20)
		{
			std::cout << "StableList (Cpp) failed test 47. Iterator range covered " << i << " elements, expected 20." << std::endl;
			return;
		}
	}

	//test 48: begin() == end() on empty list
	{
		fsds::StableList<size_t> list;
		if(list.begin() != list.end())
		{
			std::cout << "StableList (Cpp) failed test 48. begin() != end() on empty list." << std::endl;
			return;
		}
		const fsds::StableList<size_t>& clist = list;
		if(clist.begin() != clist.end())
		{
			std::cout << "StableList (Cpp) failed test 48. const begin() != end() on empty list." << std::endl;
			return;
		}
		if(list.cbegin() != list.cend())
		{
			std::cout << "StableList (Cpp) failed test 48. cbegin() != cend() on empty list." << std::endl;
			return;
		}
	}

	//test 49: const begin/end and cbegin/cend
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		const fsds::StableList<size_t>& clist = list;
		size_t i = 0;
		for(auto it = clist.begin(); it != clist.end(); ++it)
		{
			if(*it != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 49. Incorrect value via const begin()/end() at position " << i << "." << std::endl;
				return;
			}
			i++;
		}
		if(i != 20)
		{
			std::cout << "StableList (Cpp) failed test 49. Const iterator range covered " << i << " elements, expected 20." << std::endl;
			return;
		}
		i = 0;
		for(auto it = list.cbegin(); it != list.cend(); ++it)
		{
			if(*it != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 49. Incorrect value via cbegin()/cend() at position " << i << "." << std::endl;
				return;
			}
			i++;
		}
		if(i != 20)
		{
			std::cout << "StableList (Cpp) failed test 49. cbegin()/cend() range covered " << i << " elements, expected 20." << std::endl;
			return;
		}
	}

	//test 50: rbegin/rend reverse iteration
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		size_t i = 20;
		for(auto it = list.rbegin(); it != list.rend(); ++it)
		{
			i--;
			if(*it != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 50. Incorrect value via rbegin()/rend() at position " << i << ". Expected " << i + testMagicNumber << ", got " << *it << "." << std::endl;
				return;
			}
		}
		if(i != 0)
		{
			std::cout << "StableList (Cpp) failed test 50. Reverse iterator covered " << 20 - i << " elements, expected 20." << std::endl;
			return;
		}
	}

	//test 51: const rbegin/rend and rcbegin/rcend
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		const fsds::StableList<size_t>& clist = list;
		size_t i = 20;
		for(auto it = clist.rbegin(); it != clist.rend(); ++it)
		{
			i--;
			if(*it != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 51. Incorrect value via const rbegin()/rend() at position " << i << "." << std::endl;
				return;
			}
		}
		if(i != 0)
		{
			std::cout << "StableList (Cpp) failed test 51. Const reverse iterator covered wrong number of elements." << std::endl;
			return;
		}
		i = 20;
		for(auto it = list.rcbegin(); it != list.rcend(); ++it)
		{
			i--;
			if(*it != i + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 51. Incorrect value via rcbegin()/rcend() at position " << i << "." << std::endl;
				return;
			}
		}
		if(i != 0)
		{
			std::cout << "StableList (Cpp) failed test 51. rcbegin()/rcend() covered wrong number of elements." << std::endl;
			return;
		}
	}

	//test 52: iterator arithmetic (++, --, +=, -=, +, -, [])
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		auto it = list.begin();
		++it;
		if(*it != 1 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. pre-increment incorrect." << std::endl; return; }
		auto prev = it++;
		if(*prev != 1 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. post-increment returned wrong value." << std::endl; return; }
		if(*it != 2 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. post-increment did not advance." << std::endl; return; }
		--it;
		if(*it != 1 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. pre-decrement incorrect." << std::endl; return; }
		auto next = it--;
		if(*next != 1 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. post-decrement returned wrong value." << std::endl; return; }
		if(*it != 0 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. post-decrement did not move back." << std::endl; return; }
		it += 5;
		if(*it != 5 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. += incorrect." << std::endl; return; }
		it -= 3;
		if(*it != 2 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. -= incorrect." << std::endl; return; }
		auto plus = it + 4;
		if(*plus != 6 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. operator+ incorrect." << std::endl; return; }
		auto minus = plus - 2;
		if(*minus != 4 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. operator- incorrect." << std::endl; return; }
		auto friendPlus = 3 + list.begin();
		if(*friendPlus != 3 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. friend n + iterator incorrect." << std::endl; return; }
		if(list.begin()[7] != 7 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 52. operator[] incorrect." << std::endl; return; }
	}

	//test 53: iterator difference
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		if((list.end() - list.begin()) != 20)
		{
			std::cout << "StableList (Cpp) failed test 53. end() - begin() = " << (list.end() - list.begin()) << ", expected 20." << std::endl;
			return;
		}
		auto a = list.begin() + 5;
		auto b = list.begin() + 12;
		if((b - a) != 7)
		{
			std::cout << "StableList (Cpp) failed test 53. Iterator difference incorrect. Got " << (b - a) << ", expected 7." << std::endl;
			return;
		}
	}

	//test 54: iterator comparisons
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		auto a = list.begin();
		auto b = list.begin() + 5;
		auto c = list.begin();
		if(!(a == c)) { std::cout << "StableList (Cpp) failed test 54. a == c should be true." << std::endl; return; }
		if(a == b) { std::cout << "StableList (Cpp) failed test 54. a == b should be false." << std::endl; return; }
		if(!(a != b)) { std::cout << "StableList (Cpp) failed test 54. a != b should be true." << std::endl; return; }
		if(!(a < b)) { std::cout << "StableList (Cpp) failed test 54. a < b should be true." << std::endl; return; }
		if(!(b > a)) { std::cout << "StableList (Cpp) failed test 54. b > a should be true." << std::endl; return; }
		if(!(a <= c)) { std::cout << "StableList (Cpp) failed test 54. a <= c should be true." << std::endl; return; }
		if(!(a >= c)) { std::cout << "StableList (Cpp) failed test 54. a >= c should be true." << std::endl; return; }
		if(!(a <= b)) { std::cout << "StableList (Cpp) failed test 54. a <= b should be true." << std::endl; return; }
		if(!(b >= a)) { std::cout << "StableList (Cpp) failed test 54. b >= a should be true." << std::endl; return; }
	}

	//test 55: write through iterator
	{
		fsds::StableList<size_t> list(20, testMagicNumber);
		size_t i = 0;
		for(auto it = list.begin(); it != list.end(); ++it)
		{
			*it = i + testMagicNumber;
			i++;
		}
		for(size_t j = 0; j < 20; j++)
		{
			if(list.at(j) != j + testMagicNumber)
			{
				std::cout << "StableList (Cpp) failed test 55. Write through iterator did not persist at index " << j << "." << std::endl;
				return;
			}
		}
	}

	//test 56: getIterator with positive and negative step
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		{
			auto start = list.indexToIRef(0);
			auto it = list.getIterator(start, 2);
			if(*it != 0 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 56. getIterator step 2 initial value wrong." << std::endl; return; }
			++it;
			if(*it != 2 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 56. getIterator step 2 after ++ wrong." << std::endl; return; }
			++it;
			if(*it != 4 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 56. getIterator step 2 after second ++ wrong." << std::endl; return; }
		}
		{
			auto start = list.indexToIRef(19);
			auto it = list.getIterator(start, -1);
			if(*it != 19 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 56. getIterator negative step initial value wrong." << std::endl; return; }
			++it;
			if(*it != 18 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 56. getIterator negative step after ++ wrong." << std::endl; return; }
		}
	}

	//test 57: getConstIterator with positive and negative step
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		const fsds::StableList<size_t>& clist = list;
		{
			auto start = list.indexToIRef(0);
			auto it = clist.getConstIterator(start, 3);
			if(*it != 0 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 57. getConstIterator step 3 initial wrong." << std::endl; return; }
			++it;
			if(*it != 3 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 57. getConstIterator step 3 after ++ wrong." << std::endl; return; }
		}
		{
			auto start = list.indexToIRef(10);
			auto it = clist.getConstIterator(start, -2);
			if(*it != 10 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 57. getConstIterator step -2 initial wrong." << std::endl; return; }
			++it;
			if(*it != 8 + testMagicNumber) { std::cout << "StableList (Cpp) failed test 57. getConstIterator step -2 after ++ wrong." << std::endl; return; }
		}
	}

	//test 58: getUnorderedIterator and getUnorderedConstIterator visit all elements exactly once
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		std::vector<bool> seen(20, false);
		auto it = list.getUnorderedIterator();
		for(size_t i = 0; i < 20; i++)
		{
			size_t val = *it;
			if(val < testMagicNumber || val >= testMagicNumber + 20)
			{
				std::cout << "StableList (Cpp) failed test 58. getUnorderedIterator yielded out-of-range value " << val << "." << std::endl;
				return;
			}
			if(seen[val - testMagicNumber])
			{
				std::cout << "StableList (Cpp) failed test 58. getUnorderedIterator yielded duplicate value " << val << "." << std::endl;
				return;
			}
			seen[val - testMagicNumber] = true;
			++it;
		}
		for(size_t i = 0; i < 20; i++)
		{
			if(!seen[i])
			{
				std::cout << "StableList (Cpp) failed test 58. getUnorderedIterator missed value " << i + testMagicNumber << "." << std::endl;
				return;
			}
		}
		const fsds::StableList<size_t>& clist = list;
		std::vector<bool> seenConst(20, false);
		auto cit = clist.getUnorderedConstIterator();
		for(size_t i = 0; i < 20; i++)
		{
			size_t val = *cit;
			if(val < testMagicNumber || val >= testMagicNumber + 20 || seenConst[val - testMagicNumber])
			{
				std::cout << "StableList (Cpp) failed test 58. getUnorderedConstIterator yielded bad or duplicate value " << val << "." << std::endl;
				return;
			}
			seenConst[val - testMagicNumber] = true;
			++cit;
		}
		for(size_t i = 0; i < 20; i++)
		{
			if(!seenConst[i])
			{
				std::cout << "StableList (Cpp) failed test 58. getUnorderedConstIterator missed value " << i + testMagicNumber << "." << std::endl;
				return;
			}
		}
	}

	//test 59: Iterator to ConstIterator conversion
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		typename fsds::StableList<size_t>::Iterator it = list.begin();
		typename fsds::StableList<size_t>::ConstIterator cit = it;
		if(*cit != 0 + testMagicNumber)
		{
			std::cout << "StableList (Cpp) failed test 59. ConstIterator converted from Iterator has wrong value." << std::endl;
			return;
		}
		++cit;
		if(*cit != 1 + testMagicNumber)
		{
			std::cout << "StableList (Cpp) failed test 59. Advanced converted ConstIterator has wrong value." << std::endl;
			return;
		}
	}

	//test 60: iRef() and step() accessors
	{
		fsds::StableList<size_t> list;
		for(size_t i = 0; i < 20; i++) { list.append(i + testMagicNumber); }
		auto start = list.indexToIRef(5);
		auto it = list.getIterator(start, 2);
		if(it.iRef() != start)
		{
			std::cout << "StableList (Cpp) failed test 60. Iterator::iRef() returned wrong iRef." << std::endl;
			return;
		}
		if(it.step() != 2)
		{
			std::cout << "StableList (Cpp) failed test 60. Iterator::step() returned " << it.step() << ", expected 2." << std::endl;
			return;
		}
		const fsds::StableList<size_t>& clist = list;
		auto cit = clist.getConstIterator(start, -3);
		if(cit.iRef() != start)
		{
			std::cout << "StableList (Cpp) failed test 60. ConstIterator::iRef() returned wrong iRef." << std::endl;
			return;
		}
		if(cit.step() != -3)
		{
			std::cout << "StableList (Cpp) failed test 60. ConstIterator::step() returned " << cit.step() << ", expected -3." << std::endl;
			return;
		}
	}

	std::cout << "StableList (Cpp) passed all tests" << std::endl;
}

void testSlopMapC()
{
	//
}

void testSlotMapCpp()
{
	//
}

void testKTreeC()
{
	//
}

void testKTreeCpp()
{
	//
}

int main(int /*argc*/, const char** /*argv*/)
{
	testSPSCQueue();
	testFinitePQueue();
	testSeperateDataList();
	testStaticString();
	testDynamicString();
	testInlineList();

	testStableListC();
	testStableListCpp();

	testSlopMapC();
	testSlotMapCpp();

	testKTreeC();
	testKTreeCpp();

	return 0;
}