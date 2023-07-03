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

class ChunkListTestClass
{
	public:
		static size_t numAllocated;
		static size_t numCreated;
		static size_t numDestroyed;
		static size_t numBadDestructions;
		size_t val;
		int hasBeenInitialised; //314159 means constructed anything else means uninitialised

		ChunkListTestClass()
		: val(), hasBeenInitialised(314159)
		{
			ChunkListTestClass::numAllocated++;
			ChunkListTestClass::numCreated++;
		}
		ChunkListTestClass(size_t initialValue)
		: val(initialValue), hasBeenInitialised(314159)
		{
			ChunkListTestClass::numAllocated++;
			ChunkListTestClass::numCreated++;
		}
		~ChunkListTestClass()
		{
			if(this->hasBeenInitialised != 314159)
			{
				ChunkListTestClass::numBadDestructions++;
			}
			this->hasBeenInitialised = 0;
			ChunkListTestClass::numAllocated--;
			ChunkListTestClass::numDestroyed++;
		}

		ChunkListTestClass& operator=(const ChunkListTestClass& other)
		{
			if(this->hasBeenInitialised != 314159)
			{
				this->hasBeenInitialised = 314159;
				ChunkListTestClass::numAllocated++;
			}
			this->val = other.val;
			return *this;
		}
		ChunkListTestClass& operator=(const size_t& other)
		{
			this->val = other;
			return *this;
		}
};
size_t ChunkListTestClass::numAllocated = 0;
size_t ChunkListTestClass::numCreated = 0;
size_t ChunkListTestClass::numDestroyed = 0;
size_t ChunkListTestClass::numBadDestructions = 0;

void testChunkList()
{
	std::vector<size_t> testsFailed;

	if(std::is_trivially_destructible<ChunkListTestClass>::value)
	{
		std::cout << "ChunkList test is invalid as the test class is trivially destructible" << std::endl;
		testsFailed.push_back(0);
	}
	
	//test 1: constructor
	{
		auto numAllocatedBeforeTest = ChunkListTestClass::numAllocated;
		auto numCreatedBeforeTest = ChunkListTestClass::numCreated;
		auto numDestroyedBeforeTest = ChunkListTestClass::numDestroyed;
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
		}
		if((numAllocatedBeforeTest != ChunkListTestClass::numAllocated) || (numCreatedBeforeTest != ChunkListTestClass::numCreated) || (numDestroyedBeforeTest != ChunkListTestClass::numDestroyed) || (ChunkListTestClass::numBadDestructions != 0))
		{
			testsFailed.push_back(1);
		}
	}
	
	//test 2: single add
	{
		auto numAllocatedBeforeTest = ChunkListTestClass::numAllocated;
		auto numCreatedBeforeTest = ChunkListTestClass::numCreated;
		auto numDestroyedBeforeTest = ChunkListTestClass::numDestroyed;
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			auto elem = localCL.add(ChunkListTestClass(6));
			if(elem->val != 6)
			{
				testsFailed.push_back(2);
			}
		}
		if((numAllocatedBeforeTest != ChunkListTestClass::numAllocated) || (numCreatedBeforeTest + 1 != ChunkListTestClass::numCreated) || (numDestroyedBeforeTest + 2 != ChunkListTestClass::numDestroyed) || (ChunkListTestClass::numBadDestructions != 0))
		{
			testsFailed.push_back(2);
		}
	}
	
	//test 3: adds less than chunk size
	{
		auto numAllocatedBeforeTest = ChunkListTestClass::numAllocated;
		auto numCreatedBeforeTest = ChunkListTestClass::numCreated;
		auto numDestroyedBeforeTest = ChunkListTestClass::numDestroyed;
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			for(size_t i = 0; i < 4; i++)
			{
				auto elem = localCL.add(ChunkListTestClass(i+6));
				if(elem->val != i+6)
				{
					testsFailed.push_back(3);
					break;
				}
			}
		}
		if((numAllocatedBeforeTest != ChunkListTestClass::numAllocated) || (numCreatedBeforeTest + 4 != ChunkListTestClass::numCreated) || (numDestroyedBeforeTest + 8 != ChunkListTestClass::numDestroyed) || (ChunkListTestClass::numBadDestructions != 0))
		{
			testsFailed.push_back(3);
		}
	}
	
	//test 4: adds more than chunk size
	{
		auto numAllocatedBeforeTest = ChunkListTestClass::numAllocated;
		auto numCreatedBeforeTest = ChunkListTestClass::numCreated;
		auto numDestroyedBeforeTest = ChunkListTestClass::numDestroyed;
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			for(size_t i = 0; i < 40; i++)
			{
				auto elem = localCL.add(ChunkListTestClass(i+6));
				//std::cout << ">> allocated:" << ChunkListTestClass::numAllocated << ", created:" << ChunkListTestClass::numCreated << ", destroyed:" << ChunkListTestClass::numDestroyed << std::endl;
				if(elem->val != i+6)
				{
					testsFailed.push_back(4);
					break;
				}
			}
		}
		
		if((numAllocatedBeforeTest != ChunkListTestClass::numAllocated) || (numCreatedBeforeTest + 40 != ChunkListTestClass::numCreated) || (numDestroyedBeforeTest + 80 != ChunkListTestClass::numDestroyed) || (ChunkListTestClass::numBadDestructions != 0))
		{
			testsFailed.push_back(4);
		}
	}
	
	//test 5: single add and remove
	{
		auto numAllocatedBeforeTest = ChunkListTestClass::numAllocated;
		auto numCreatedBeforeTest = ChunkListTestClass::numCreated;
		auto numDestroyedBeforeTest = ChunkListTestClass::numDestroyed;
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			auto elem = localCL.add(ChunkListTestClass(6));
			if(elem->val != 6)
			{
				testsFailed.push_back(5);
			}
			localCL.remove(elem);

			if((numAllocatedBeforeTest != ChunkListTestClass::numAllocated) || (numCreatedBeforeTest + 1 != ChunkListTestClass::numCreated) || (numDestroyedBeforeTest + 2 != ChunkListTestClass::numDestroyed) || (ChunkListTestClass::numBadDestructions != 0))
			{
				testsFailed.push_back(5);
			}
		}
	}
	
	//test 6: adds less than chunk size then remove
	{
		auto numAllocatedBeforeTest = ChunkListTestClass::numAllocated;
		auto numCreatedBeforeTest = ChunkListTestClass::numCreated;
		auto numDestroyedBeforeTest = ChunkListTestClass::numDestroyed;
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			std::array<ChunkListTestClass*, 4> elems;
			for(size_t i = 0; i < 4; i++)
			{
				elems[i] = localCL.add(ChunkListTestClass(i+6));
				if(elems[i]->val != i+6)
				{
					testsFailed.push_back(6);
					break;
				}
			}
			for(size_t i = 0; i < 4; i++)
			{
				localCL.remove(elems[i]);
			}

			if((numAllocatedBeforeTest != ChunkListTestClass::numAllocated) || (numCreatedBeforeTest + 4 != ChunkListTestClass::numCreated) || (numDestroyedBeforeTest + 8 != ChunkListTestClass::numDestroyed) || (ChunkListTestClass::numBadDestructions != 0))
			{
				testsFailed.push_back(6);
			}
		}
	}
	
	//test 7: adds less than chunk size alternating with removing
	{
		auto numAllocatedBeforeTest = ChunkListTestClass::numAllocated;
		auto numCreatedBeforeTest = ChunkListTestClass::numCreated;
		auto numDestroyedBeforeTest = ChunkListTestClass::numDestroyed;
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			for(size_t i = 0; i < 4; i++)
			{
				auto elem = localCL.add(ChunkListTestClass(i+6));
				if(elem->val != i+6)
				{
					testsFailed.push_back(7);
					break;
				}
				localCL.remove(elem);
			}
			
			if((numAllocatedBeforeTest != ChunkListTestClass::numAllocated) || (numCreatedBeforeTest + 4 != ChunkListTestClass::numCreated) || (numDestroyedBeforeTest + 8 != ChunkListTestClass::numDestroyed) || (ChunkListTestClass::numBadDestructions != 0))
			{
				testsFailed.push_back(7);
			}
		}
	}
	
	//test 8: adds more than chunk size alternating with removing
	{
		auto numAllocatedBeforeTest = ChunkListTestClass::numAllocated;
		auto numCreatedBeforeTest = ChunkListTestClass::numCreated;
		auto numDestroyedBeforeTest = ChunkListTestClass::numDestroyed;
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			for(size_t i = 0; i < 40; i++)
			{
				auto elem = localCL.add(ChunkListTestClass(i+6));
				if(elem->val != i+6)
				{
					testsFailed.push_back(8);
					break;
				}
				localCL.remove(elem);
			}
			
			if((numAllocatedBeforeTest != ChunkListTestClass::numAllocated) || (numCreatedBeforeTest + 40 != ChunkListTestClass::numCreated) || (numDestroyedBeforeTest + 80 != ChunkListTestClass::numDestroyed) || (ChunkListTestClass::numBadDestructions != 0))
			{
				testsFailed.push_back(8);
			}
		}
	}
	
	//test 9: adds more than chunk size then remove
	{
		auto numAllocatedBeforeTest = ChunkListTestClass::numAllocated;
		auto numCreatedBeforeTest = ChunkListTestClass::numCreated;
		auto numDestroyedBeforeTest = ChunkListTestClass::numDestroyed;
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			std::array<ChunkListTestClass*, 40> elems;
			for(size_t i = 0; i < 40; i++)
			{
				elems[i] = localCL.add(ChunkListTestClass(i+6));
				if(elems[i]->val != i+6)
				{
					testsFailed.push_back(9);
					break;
				}
			}
			for(size_t i = 0; i < 40; i++)
			{
				localCL.remove(elems[i]);
			}
			
			if((numAllocatedBeforeTest != ChunkListTestClass::numAllocated) || (numCreatedBeforeTest + 40 != ChunkListTestClass::numCreated) || (numDestroyedBeforeTest + 80 != ChunkListTestClass::numDestroyed) || (ChunkListTestClass::numBadDestructions != 0))
			{
				testsFailed.push_back(9);
			}
		}
	}
	
	//test 10: is empty
	{
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			auto elem = localCL.add(ChunkListTestClass(6));
			if(localCL.isEmpty() == true)
			{
				testsFailed.push_back(10);
			}
			localCL.remove(elem);
			if(localCL.isEmpty() == false)
			{
				testsFailed.push_back(10);
			}
		}
	}
	
	//test 11: size
	{
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			std::array<ChunkListTestClass*, 40> elems;
			for(size_t i = 0; i < 40; i++)
			{
				elems[i] = localCL.add(ChunkListTestClass(i+6));
				if(localCL.size() != i+1)
				{
					testsFailed.push_back(11);
					break;
				}
			}
			for(size_t i = 0; i < 40; i++)
			{
				localCL.remove(elems[i]);
				if(localCL.size() != 40-i-1)
				{
					testsFailed.push_back(11);
					break;
				}
			}
		}
	}
	
	//test 12: clear
	{
		auto numAllocatedBeforeTest = ChunkListTestClass::numAllocated;
		auto numCreatedBeforeTest = ChunkListTestClass::numCreated;
		auto numDestroyedBeforeTest = ChunkListTestClass::numDestroyed;
		{
			fsds::ChunkList<ChunkListTestClass, 16> localCL;
			for(size_t i = 0; i < 40; i++)
			{
				auto elem = localCL.add(ChunkListTestClass(i+6));
				if(elem->val != i+6)
				{
					testsFailed.push_back(12);
					break;
				}
			}
			localCL.clear();

			if((numAllocatedBeforeTest != ChunkListTestClass::numAllocated) || (numCreatedBeforeTest + 40 != ChunkListTestClass::numCreated) || (numDestroyedBeforeTest + 80 != ChunkListTestClass::numDestroyed) || (ChunkListTestClass::numBadDestructions != 0))
			{
				testsFailed.push_back(12);
			}
		}
	}
	
	if(testsFailed.size() == 0)
	{
		std::cout << "ChunkList passed all tests" << std::endl;
	}
	else
	{
		if(testsFailed.size() == 1)
		{
			std::cout << "ChunkList failed test " << testsFailed[0] << std::endl;
		}
		else
		{
			std::cout << "ChunkList failed tests ";
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
	testStaticString();
	testDynamicString();
	testChunkList();

	return 0;
}