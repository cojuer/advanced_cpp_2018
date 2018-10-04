#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <vector>

template<typename T1, typename T2>
auto Compose(T1 func1, T2 func2)
{
    return [func1, func2](auto param) {
        return func1(func2(param));
    };
}

template<typename T1>
auto ComposeMultiple(T1 func)
{
    return func;
}

template<typename T1, typename ... Types>
auto ComposeMultiple(T1 func1, Types ... args)
{
    auto recursive = ComposeMultiple(args...);
    return [func1, recursive](auto param) {
        return func1(recursive(param));
    };
}

const char* f2(const std::string& str) {
    return str.c_str();
}

double f0(double val) {
    return val + 1;
}

int main() {
    std::string s[] = {"1.2", "2.343", "3.2"};
    std::vector<double> d(3);
    std::vector<double> d2(3);
    auto f1 = atof;
    std::transform(s, s + 3, d.begin(), Compose(f1, f2));
    for (auto elem : d)
    {
        std::cout << elem << ',';
    }
    std::cout << std::endl;
    std::transform(s, s + 3, d2.begin(), ComposeMultiple(f0, f1, f2));
    for (auto elem : d2)
    {
        std::cout << elem << ',';
    }
    std::cout << std::endl;
}