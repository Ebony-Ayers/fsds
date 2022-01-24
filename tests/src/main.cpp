#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>

#include "../../src/fsds.hpp"


int main(int /*argc*/, const char** /*argv*/)
{	
	fsds::ts_List<int> l;

	l.append(1);
	l.append(2);
	l.append(3);

	for(size_t i = 0; i < l.size(); i++)
	{
		std::cout << (*l[i]) << std::endl;
	}
	
	return 0;
}
