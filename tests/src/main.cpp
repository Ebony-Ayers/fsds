#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <vector>

#define FSDS_DEBUG
#include "../../src/fsds.hpp"

void testFinitePQueue()
{
	size_t testsFailed = 0;

	size_t testSize = 100;
	size_t numPriorities = 6;
	fsds::FinitePQueue<size_t, 6> pqueue;

	//test 1: simple enqueue and dequeue
	for(size_t i = 0; i < testSize; i++)
	{
		pqueue.enqueue(i, 5);
	}
	//for(size_t i = 0; i < 100; i++)
	//{
	//	pqueue.makeNewBlock();
	//}
	for(size_t i = 0; i < testSize; i++)
	{
		auto result = pqueue.dequeue(5);
		if(result != i)
		{
			testsFailed++;
			break;
		}
	}

	
	//test 2: check that after enqueuing and dequeuing the same number of elements from an empty queue the size of the queue is 0
	if(pqueue.size() != 0)
	{
		testsFailed++;
	}

	/*
	//test 3: check enough blocks have been allocated
	if(pqueue.capacity() != 1008)
	{
		testsFailed++;
	}
	*/

	//test 4: check the queue has been reset
	pqueue.clear();
	if((pqueue.capacity() != 48) && (pqueue.size() == 0))
	{
		testsFailed++;
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
			testsFailed++;
			break;
		}
	}
	
	if(testsFailed == 0)
	{
		std::cout << "FinitePQueue passed all tests" << std::endl;
	}
	else
	{
		std::cout << "FinitePQeueue failed " << testsFailed << " tests" << std::endl;
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

int main(int /*argc*/, const char** /*argv*/)
{
	/*
	fsds::SPMCQueue<int> q(5);

	for(int i = 0; i < 20; i++)
	{
		q.enqueue(i);
	}
	std::cout << "front: " << q.front() << std::endl;
	for(int i = 0; i < 4; i++)
	{
		std::cout << q.dequeue() << std::endl;
	}
	for(int i = 0; i < 4; i++)
	{
		q.enqueue(i + 20);
	}
	for(int i = 0; i < 20; i++)
	{
		int dest;
		q.tryDequeue(&dest);
		std::cout << dest << std::endl;
	}
	for(int i = 0; i < 4; i++)
	{
		q.enqueue(i + 24);
	}
	for(size_t i = 0; i < 4; i++)
	{
		std::cout << q[i] << std::endl;
	}
	for(int i = 0; i < 4; i++)
	{
		std::cout << q.dequeue() << std::endl;
	}
	std::cout << "is empty " << q.isEmpty() << std::endl;

	fsds::SPMCQueue<int> p;
	for(int i = 0; i < 4; i++)
	{
		p.enqueue(i);
	}
	
	std::cout << "equality: " << (q == p) << std::endl;

	std::cout << "size " << q.size() << std::endl;
	std::cout << "capacity " << q.capacity() << std::endl;
	q.reserve(2);
	p.clear();

	std::cout << "equality: " << (q == p) << std::endl;
	*/
	
	//testFinitePQueue();
	testSPSCQueue();

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