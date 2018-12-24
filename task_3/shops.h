#include <atomic>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <iostream>
#include <memory>
#include <chrono>
#include <map>

using namespace std::chrono_literals;

class IProduct;

class IShop
{
public:
    virtual void AddProduct(IProduct& product) = 0;
    virtual void DelProduct(IProduct& product) = 0;

    virtual void SellAll() const = 0;
};

class IProduct : public std::enable_shared_from_this<IProduct>
{
public:
    virtual void ChangePrice(double value) = 0;
    virtual double GetPrice() const = 0;

    virtual std::string GetType() const = 0;

    virtual void StartSales() = 0;
    virtual void StopSales() = 0;

    virtual bool OnSale() const = 0;

    virtual void Attach(IShop* shop) = 0;
    virtual void Detach(IShop* shop) = 0;
};

class IShopImpl : public IShop
{
public:
    IShopImpl(int number) : m_number(number)
    {}

    virtual ~IShopImpl() = default;

    void AddProduct(IProduct& product)
    {
        std::unique_lock<std::mutex> m(m_prod_guard);
        m_products[product.GetType()] = product.shared_from_this();
    }

    void DelProduct(IProduct& product)
    {
        std::unique_lock<std::mutex> m(m_prod_guard);
        m_products.erase(product.GetType());
    }

    void SellAll() const
    {
        std::unique_lock<std::mutex> m(m_prod_guard);
        for (auto iter : m_products)
        {
            // if started sale and product still exists, 
            // shop prolongs lifetime of product using shared_ptr
            auto sh_product = iter.second.lock();
            if (sh_product && sh_product->OnSale()) 
            {
                std::cout << m_number << " sell " << sh_product->GetType()  << ": " << sh_product->GetPrice() << std::endl;
            }
        }
    }

    double Sell(std::string type)
    {
        std::unique_lock<std::mutex> m(m_prod_guard);
        if (m_products.find(type) != m_products.end()) 
        {
            auto sh_product = m_products[type].lock();
            if (sh_product && sh_product->OnSale()) 
            {
                return sh_product->GetPrice();
            }
        }
        return -1;
    }
private:
    int m_number;
    mutable std::mutex m_prod_guard;
    std::map<std::string, std::weak_ptr<IProduct>> m_products;
};

class IProductImpl : public IProduct
{
public:
    IProductImpl(double price) : m_price(price)
    {}

    ~IProductImpl()
    {
        StopSales();
    }

    void ChangePrice(double value)
    {
        m_price = value;
    }
    
    double GetPrice() const
    {
        return m_price;
    }

    void StartSales()
    {
        m_on_sale = true;
    }
    void StopSales()
    {
        m_on_sale = false;
    }

    bool OnSale() const 
    {
        return m_on_sale;
    }

    void Attach(IShop* shop) 
    {
        if (shop) {
            shop->AddProduct(*this);
        }
    }

    void Detach(IShop* shop)
    {
        if (shop) {
            shop->DelProduct(*this);
        }
    }
private:
    std::atomic<double> m_price;
    std::atomic<bool> m_on_sale;
};

class A : public IProductImpl {
public: 
    A(double price) : IProductImpl(price) {};
    std::string GetType() const { return "A"; };
};
class B : public IProductImpl {
public: 
    B(double price) : IProductImpl(price) {};
    std::string GetType() const { return "B"; };
};
class C : public IProductImpl {
public: 
    C(double price) : IProductImpl(price) {};
    std::string GetType() const { return "C"; };
};