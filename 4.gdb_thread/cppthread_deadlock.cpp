#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>
#include <unistd.h> //for linux sleep()

std::mutex g_mutex;

class Foo 
{
	public:
		Foo(int m)
		{
			m_data = m;
		}
		~Foo(){}
		void printval() 
		{
			std::cout << "m_data = " << m_data << std::endl;
		}
		void increase()
		{
			g_mutex.lock(); //故意制造双重加锁
			++m_data;
			g_mutex.unlock();
		}
		int getval()
		{
			return m_data;
		}
		void resetval()
		{
			m_data = 0;	
		}
		
	private:
		int m_data;
};

void thread_func1(Foo& p)
{
	while (true)
	{
		g_mutex.lock();
		p.increase();
		if(p.getval() == 1024)
			p.resetval();
		g_mutex.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void thread_func2(Foo& p)
{
	while (true)
	{
		g_mutex.lock();
		p.printval();
		g_mutex.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main()
{	
	Foo *pFoo = new Foo(0);

	std::thread t1(thread_func1, std::ref(*pFoo)); //std::ref用于std::thread传入参数，以引用的形式
	std::thread t2(thread_func2, std::ref(*pFoo));

	t1.join();
	t2.join();
	
	delete pFoo;
	pFoo = NULL;
			
	return 0;
}
