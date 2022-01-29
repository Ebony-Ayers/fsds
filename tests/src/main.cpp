#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>

#include "../../src/fsds.hpp"


int main(int /*argc*/, const char** /*argv*/)
{	
	fsds::SPSCQueue<int> q;

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
	
	return 0;
}
