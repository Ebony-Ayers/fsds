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
	const char* baseStr = "哈的是 энергия буран поезд поезда";
	const char* subStr = "поезда";
	const char* letterW = "w";
	const char* testCStr = "поезда";
	const char* startCStr = "哈的是";

	fsds::DynamicString str = fsds::DynamicString(reinterpret_cast<const fsds::DynamicString::CharT*>(baseStr), 30);
	fsds::DynamicString smallStr = fsds::DynamicString(reinterpret_cast<const fsds::DynamicString::CharT*>(subStr), 6);
	fsds::DynamicString singleCharacter = fsds::DynamicString(reinterpret_cast<const fsds::DynamicString::CharT*>(letterW), 1);
	fsds::DynamicString testStr = fsds::DynamicString(reinterpret_cast<const fsds::DynamicString::CharT*>(testCStr), 6);
	fsds::DynamicString startStr = fsds::DynamicString(reinterpret_cast<const fsds::DynamicString::CharT*>(startCStr), 3);

	
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