#include <iostream>
#include <type_traits>
#include <memory>

class BaseHolder
{
public:
    virtual ~BaseHolder() = default;
};
using BaseHolderPtr = std::unique_ptr<BaseHolder>;

template <typename T>
class Holder : public BaseHolder
{
public:
    Holder(T value) : m_value(value) 
    {}

    T get() const
    {
        return m_value;
    }
private:
    T m_value;
};

class Any
{
public:
    template <typename T>
    Any(T value) : m_holder(new Holder<T>(value)) 
    {}

    template <typename T>
    T get()
    {
        auto casted = dynamic_cast<Holder<T>*>(m_holder.get());
        if (casted) {
            return casted->get();
        } else {
            throw std::exception();
        }
    }
private:
    BaseHolderPtr m_holder;
};

int main(void)
{
    Any a(5);
    a.get<int>();
    a.get<std::string>();
    return 0;
}