#include <cstdlib>
#include <functional>
#include <iostream>

template<typename T1, typename T2>
auto Compose(T1 func1, T2 func2)
{
    return [func1, func2](auto param) {
        return func1(func2(param));
    };
}

const char* f2(const std::string& str) {
    return str.c_str();
}

int main() {
    std::string s[] = {"1.2", "2.343", "3.2"};
    std::vector<double> d(3);
    auto f1 = atof;
    std::transform(s, s + 3, d.begin(), Compose(f1, f2));
    for (auto elem : d)
    {
        std::cout << elem << std::endl;
    }
}