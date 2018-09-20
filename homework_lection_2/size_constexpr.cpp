#include <iostream>

template<typename T, size_t sz>
constexpr uint32_t Size(T (&t)[sz])
{
	return sz;
}

int main()
{
	int a[1005];
	double b[Size(a)];
	std::cout << Size(b) << std::endl;
    return 0;
}