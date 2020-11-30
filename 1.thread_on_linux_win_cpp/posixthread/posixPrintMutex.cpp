
#include <pthread.h>
#include <iostream>
#include <unistd.h>

int g_mydata = 1;
pthread_mutex_t m;

void* thread_function1(void* args)
{
	while (g_mydata < INT_MAX)
	{
		pthread_mutex_lock(m);
		++g_mydata;
		pthread_mutex_unlock(m);
	}
	
	return NULL;
} 

void* thread_function2(void* args)
{
	while (g_mydata < INT_MAX)
	{	
		pthread_mutex_lock(m);
		std::cout << "g_mydata = " << g_mydata << ", ThreadID: " << pthread_self() << std::endl;
		pthread_mutex_unlock(m);
		
		sleep(1);
	}
	
	return NULL;
} 

int main()
{
	pthread_mutex_init(m, NULL);
	
	pthread_t threadIDs[2];	
	pthread_create(&threadIDs[0], NULL, thread_function1, NULL);
	pthread_create(&threadIDs[1], NULL, thread_function2, NULL);
	
	for(int i = 0; i < 2; ++i)
	{
		pthread_join(threadIDs[i], NULL);
	}
	
	pthread_mutex_destroy(m);

	return 0;
}