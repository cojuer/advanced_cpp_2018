#include <iostream>
#include <vector>

#define STACK_SIZE 40

template <class T>
struct TestAllocator {
public:
    using value_type = T;

    T* allocate (size_t n) 
    {   
        if (n == 0) return nullptr;
        if (!m_is_buf_used && n * sizeof(T) < STACK_SIZE) {
            std::cout << "alloc " << n * sizeof(T) << " static memory at " << (void*)m_buf << std::endl;
            m_is_buf_used = true;
            return (T*)(m_buf);
        }
        void* mem = operator new(n * sizeof(T)); 
        std::cout << "alloc " << n * sizeof(T) << " dynamic memory at " << mem << std::endl;
        return (T*)mem;
    }

    void deallocate (void* p, size_t)
    {
        if (p == m_buf) {
            std::cout << "dealloc static memory at " << p << std::endl;
            m_is_buf_used = false;
        }
        else if (p) {
            operator delete(p);
            std::cout << "dealloc dynamic memory at " << p << std::endl;
        }    
    }
private:
    char m_buf[STACK_SIZE];
    bool m_is_buf_used = false;
};

int main(void)
{
    std::vector<int, TestAllocator<int>> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    return 0;
}


