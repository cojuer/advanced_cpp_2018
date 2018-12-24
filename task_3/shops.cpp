#include <atomic>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <iostream>
#include <memory>
#include <chrono>
#include <map>

#include "shops.h"

using namespace std::chrono_literals;

int main(int argc, char** argv)
{
    IShopImpl* shopPtr1 = new IShopImpl{1};   
    IShopImpl* shopPtr2 = new IShopImpl{2};
    IShopImpl* shopPtr3 = new IShopImpl{3};

    std::thread products([&]() {
        auto prod1 = std::make_shared<A>(15);
        prod1->StartSales();
        prod1->Attach(shopPtr1);
        prod1->Attach(shopPtr2);
        std::this_thread::sleep_for(1s);
        auto prod2 = std::make_shared<B>(13);
        prod2->StartSales();
        prod2->Attach(shopPtr3);
        prod2->Attach(shopPtr1);
        std::this_thread::sleep_for(1s);
        prod1->Detach(shopPtr2);
        prod2->Detach(shopPtr3);
        prod2->ChangePrice(12.99);
        prod1->ChangePrice(16);
        std::this_thread::sleep_for(1s);
        auto prod3 = std::make_shared<C>(45);
        prod3->Attach(shopPtr1);
    });

    std::this_thread::sleep_for(0.5s);
    for (auto i = 0; i < 4; ++i) {
        shopPtr1->SellAll();
        shopPtr2->SellAll();
        shopPtr3->SellAll();
        std::cout << "------------" << std::endl;
        std::this_thread::sleep_for(1s);
    }

    products.join();

    delete(shopPtr1);
    delete(shopPtr2);
    delete(shopPtr3);

    return 0;
}
