#include <iostream>

#include "../../src/fsds.hpp"

int main(int /*argc*/, const char** /*argv*/)
{
	fsds::basic_list<int> l;

	l.push_back(1);
	l.push_back(2);
	l.push_back(3);
	l.push_back(4);
	l.push_back(5);
	l.resize(3);
	l.resize(6, 69);
	l.pop_back();
	l.clear();

	for(size_t i = 0; i < l.size(); i++)
	{
		std::cout << l[i] << std::endl;
	}

	std::cout << std::endl;
	std::cout << l.size() << std::endl;
	std::cout << l.capacity() << std::endl;
	
	return 0;
}
