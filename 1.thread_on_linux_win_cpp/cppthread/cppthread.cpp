#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>

int g_mydata = 1;
std::mutex g_mutex;

void thread_func1()
{
	while (g_mydata < INT_MAX)
	{
		g_mutex.lock();
		++g_mydata;
		g_mutex.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void thread_func2()
{
	while (g_mydata < INT_MAX)
	{
		g_mutex.lock();
		std::cout << "g_mydata = " << g_mydata << ", ThreadID = " << std::this_thread::get_id() << std::endl;
		g_mutex.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main()
{
	std::thread t1(thread_func1);
	std::thread t2(thread_func2);

	t1.join();
	t2.join();

	return 0;
}