#include "ThreadPool.h"
#include <iostream>

int main()
{
    ThreadPool pool(4);

    auto future1 = pool.submit([]()
        {
            return 10;
        });

    auto future2 = pool.submit([](int a, int b)
        {
            return a + b;
        }, 20, 30);

    auto future3 = pool.submit([]()
        {
            std::cout << "Hello from worker thread\n";
        });

    std::cout << future1.get() << '\n';
    std::cout << future2.get() << '\n';

    future3.get();

    return 0;
}