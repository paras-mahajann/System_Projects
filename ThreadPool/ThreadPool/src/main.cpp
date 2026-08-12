#include "ThreadPool.h"

#include<iostream>
#include<thread>
#include<chrono>

int main() {
	ThreadPool pool(4);

	for (int i = 0;i < 10;++i) {
		pool.submit([i] {
			std::cout << "Task " << i
				<< " running on thread "
				<< std::this_thread::get_id()
				<< '\n';

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			});
	}
	
	std::cin.get();
}