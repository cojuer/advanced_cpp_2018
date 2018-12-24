#include <gtest/gtest.h>

#include "shops.h"

// g++ -std=c++17 -g -pthread shops.cpp -lgtest_main -lgtest -lpthread 
// Used libgtest-dev package on Ubuntu. After apt installation:
// cd /usr/src/gtest/
// sudo cmake CMakeLists.txt 
// sudo make
// sudo cp *.a /usr/lib 

class Test : public ::testing::Test {
public:
    Test() {}
    ~Test() {}
protected:
};

TEST_F(Test, test1) {
    /** Product not attached -> shop.Sell returns -1
     */
    IShopImpl shop{ 1 };
    ASSERT_EQ(shop.Sell("A"), -1);
}

TEST_F(Test, test2) {
    /** Sales not started -> shop.Sell returns -1
     */
    IShopImpl shop{ 1 };
    auto product = std::make_shared<A>(10.0);
    product->Attach(&shop);
    ASSERT_EQ(shop.Sell(product->GetType()), -1);
}

TEST_F(Test, test3) {
    /** Sales started -> shop.Sell returns price of product
     */
    IShopImpl shop{ 1 };
    auto product = std::make_shared<A>(10.0);
    product->Attach(&shop);
    product->StartSales();
    ASSERT_EQ(shop.Sell(product->GetType()), product->GetPrice());
}

TEST_F(Test, test4) {
    /** Sales stopped -> shop.Sell returns -1
     */
    IShopImpl shop{ 1 };
    auto product = std::make_shared<A>(10.0);
    product->Attach(&shop);
    product->StartSales();
    product->StopSales();
    ASSERT_EQ(shop.Sell(product->GetType()), -1);
}


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
