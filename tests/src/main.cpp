#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>

#include "../../src/fsds.hpp"

int main(int /*argc*/, const char** /*argv*/)
{	
	
	fsds::SPMCQueue<int> q;

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

	/*
	for(int i = 0; i < 6; i++)
	{
		q.enqueue(i);
	}
	for(int i = 0; i < 4; i++)
	{
		q.dequeue();
	}
	for(int i = 6; i < 20; i++)
	{
		q.enqueue(i);
	}

	std::cout << "finished adding" << std::endl;

	for(int i = 0; i < 24; i++)
	{
		std::cout << q.dequeue() << std::endl;
	}
	*/

	/*
	const char* baseStr = "哈的是 энергия буран поезд поезда";
	const char* subStr = "поезда";

	fsds::StaticString str = fsds::StaticString(reinterpret_cast<const fsds::StaticString::CharT*>(baseStr), 30);
	fsds::StaticString smallStr = fsds::StaticString(reinterpret_cast<const fsds::StaticString::CharT*>(subStr), 6);
	
	
	std::cout << smallStr.numCodePointsInFirstCharacter() << std::endl;

	//return 0;

	std::cout << smallStr << std::endl;

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
	*/

	return 0;
}