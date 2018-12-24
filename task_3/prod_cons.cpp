#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cassert>

int main() {
    std::mutex m;
    std::condition_variable cvar;
    size_t count = 0;
    bool done = false;
    bool notified = false;
    std::queue<int> items;
    
    std::thread producer([&]() {
        for (int i = 0; i < 10000; ++i) {
            std::unique_lock<std::mutex> l(m);
            if (i != 0) { 
                while (!notified) {
                    cvar.wait(l);
                }
                notified = false;
            }
            items.push(i);
            count++;
        }
        done = true;
    });

    std::thread consumer([&]() {
        while (!done) {
            while (!items.empty()) {
                items.pop();
                std::unique_lock<std::mutex> l(m);
                notified = true;
                cvar.notify_one();
                count--;
            }
        }
    });
    
    producer.join();
    consumer.join();
    
    std::cout << count << std::endl;
}
