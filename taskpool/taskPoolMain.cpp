#include "TaskPool.h"
#include <chrono>

int main()
{
    TaskPool threadPool;
    threadPool.init(); //初始化线程对象队列

    Task* task = NULL;
    for (int i = 0; i < 10; ++i)
    {
        task = new Task();
        threadPool.addTask(task); //初始化任务对象队列，调度线程时会取出执行
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));

    threadPool.stop(); //等待所有工作线程结束

    return 0; //析构
}