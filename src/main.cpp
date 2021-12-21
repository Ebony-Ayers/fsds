#include <iostream>

int main(int argc, const char* argv[])
{
	std::cout << "Hello World!" << std::endl;

	//example of compiler warnings
	int32_t var1 = 0;
	int64_t var2 = (int64_t)var1;
	
	return 0;
}