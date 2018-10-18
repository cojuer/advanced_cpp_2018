#include <iostream>

template <typename T>
class TRefCounter
{
public:
    TRefCounter() : m_ref_cnt(0) {}

    uint32_t& ref_cnt()
    {
        return m_ref_cnt;
    }
private:
    uint32_t m_ref_cnt;
};

template <typename T>
class TBasePtr
{
public:
    TBasePtr(T* ptr = nullptr)
    {
        m_ptr = ptr;
    }

    T* operator->() const
    {
        return m_ptr;
    }

    T& operator*() const
    {
        return *m_ptr;
    }

    bool operator==(const TBasePtr& rhs) const
    {
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(const TBasePtr& rhs) const
    {
        return !this->operator==(rhs);
    }

    operator bool() const 
    {
        return m_ptr;
    }
protected:
    T* m_ptr;
};

template <typename T>
class TIntrusivePtr : public TBasePtr<T>
{
public:
    TIntrusivePtr(T* ptr): TBasePtr<T>(ptr)
    {
        if (this->m_ptr) {
            this->m_ptr->ref_cnt() += 1;
        }
    }

    TIntrusivePtr(const TIntrusivePtr& rhs)
    {
        Reset(rhs.m_ptr);
    }

    const TIntrusivePtr& operator=(const TIntrusivePtr& rhs)
    {
        Reset(rhs.m_ptr);
        return *this;
    }

    TIntrusivePtr(TIntrusivePtr&& rhs)
    {
        Reset(rhs.m_ptr);
        rhs.Release();
    }

    const TIntrusivePtr& operator=(TIntrusivePtr&& rhs)
    {
        Reset(rhs.m_ptr);
        rhs.Release();
        return *this;
    }

    ~TIntrusivePtr() 
    {
        Release();
    }

    uint32_t UseCount() const
    {
        return this->m_ptr->ref_cnt();
    }

    T* Get() const
    {
        return this->m_ptr;
    }

    void Reset(T* ptr)
    {
        if (this->m_ptr) {
            this->m_ptr->ref_cnt() -= 1;
        }
        if (this->m_ptr && this->m_ptr->ref_cnt() == 0) {
            std::cout << "delete" << std::endl;
            delete(this->m_ptr);
        }
        this->m_ptr = ptr;
        if (this->m_ptr) {
            this->m_ptr->ref_cnt() += 1;
        }
    }

    void Release()
    {
        Reset(nullptr);
    }
};

class TDoc: public TRefCounter<TDoc> {};

int main(void)
{
    TDoc* p = new TDoc;
    std::cout << "before " << p->ref_cnt() << std::endl;
    // test 1st
    TIntrusivePtr<TDoc> ip1(p);
    std::cout << "create " << p->ref_cnt() << std::endl;
    // test 2nd
    TIntrusivePtr<TDoc> ip2(p);
    std::cout << "create " << p->ref_cnt() << std::endl;
    // test copy constr
    TIntrusivePtr<TDoc> ip3 = ip2;
    std::cout << "copy " << p->ref_cnt() << std::endl;
    // test move constr
    TIntrusivePtr<TDoc> ip4(std::move(ip3));
    std::cout << "move " << p->ref_cnt() << std::endl;
    // test copy assignment
    TIntrusivePtr<TDoc> ip5(nullptr);
    ip5 = ip4;
    std::cout << "copy assign " << p->ref_cnt() << std::endl;
    // test move assignment
    TIntrusivePtr<TDoc> ip6(nullptr);
    ip6 = std::move(ip5);
    std::cout << "move assign " << p->ref_cnt() << std::endl;
    // test release
    ip1.Release();
    std::cout << "release " << p->ref_cnt() << std::endl;
    // test reset
    ip2.Reset(nullptr);
    std::cout << "reset " << p->ref_cnt() << std::endl;
    // check equal
    std::cout << "ip1: " << (bool(ip1) ? "True" : "False") << std::endl;
    std::cout << "ip4: " << (bool(ip4) ? "True" : "False") << std::endl;
    std::cout << "ip6: " << (bool(ip6) ? "True" : "False") << std::endl;
    std::cout << "ip4 == ip6: " << (bool(ip4 == ip6) ? "True" : "False") << std::endl;
    std::cout << "ip4 != ip6: " << (bool(ip4 != ip6) ? "True" : "False") << std::endl;
    std::cout << "ip1 != ip6: " << (bool(ip1 != ip6) ? "True" : "False") << std::endl;
    // release all
    ip4.Release();
    ip6.Release();
    return 0;
}