#include "ThreadPool.h"
#include <iostream>
#include <string>
void testfun(std::string s)
{
	std::cout << s << std::endl;
}

int main()
{
	ThreadPool pool(5);
	
	pool.enqueue(testfun, "I am in test fun");

	auto a = pool.enqueue(
		[](int n) {return n * 2; },
		5
	); 
	
	//enqueue will return you a std::future object. So you have to use .get() to get actual value.
	std::cout << a.get() << std::endl;
	int ab;
	std::cin >> ab;
}
