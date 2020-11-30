/**
 * 任务池模型，TaskPool.cpp
 */
#include "TaskPool.h"

TaskPool::TaskPool() : m_bRunning(false)
{

}

TaskPool::~TaskPool()
{
    removeAllTasks();
}

void TaskPool::init(int threadNum/* = 5*/)
{
    if (threadNum <= 0)
        threadNum = 5;

    m_bRunning = true;

    for (int i = 0; i < threadNum; ++i)
    {
        std::shared_ptr<std::thread> spThread;
        //shared_ptr.reset带参数是初始化，指向new出的thread对象
        //bind绑定了thread对象和其执行函数threadFunc
        spThread.reset(new std::thread(std::bind(&TaskPool::threadFunc, this))); 
        m_threads.push_back(spThread); //thread对象入队
    }
}

void TaskPool::threadFunc() //thread对象唤醒时执行
{
    std::shared_ptr<Task> spTask;
    while (true)
    {
        std::unique_lock<std::mutex> guard(m_mutexList); //RAII实现，作用域结束自动解锁
        while (m_taskList.empty())
        {                 
            if (!m_bRunning)
                break;
            
            //如果获得了互斥锁，但是条件不合适的话，pthread_cond_wait会释放锁，不往下执行。
            //当发生变化后，条件合适，pthread_cond_wait将直接获得锁。
            m_cv.wait(guard);
        }

        if (!m_bRunning)
            break;

        spTask = m_taskList.front(); //取m_taskList的task对象
        m_taskList.pop_front(); //更新m_taskList

        if (spTask == NULL)
            continue;

        spTask->doIt(); //执行task
        spTask.reset(); //shared_ptr.reset不带参数，指向对象的计数-1
    }

    std::unique_lock<std::mutex> guard(m_mutexList); //为了打印的原子性，再加锁
    {
        std::cout << "Exit thread, threadID: " << std::this_thread::get_id() << std::endl;
    }
    
}

void TaskPool::stop()
{
    m_bRunning = false;
    m_cv.notify_all(); //唤醒所有等待条件变量的线程

    //等待所有线程退出
    for (auto& iter : m_threads)
    {
        if (iter->joinable())   //该线程是否可join
            iter->join();       //主线程等待该线程
    }
}

void TaskPool::addTask(Task* task)
{
    std::shared_ptr<Task> spTask;
    spTask.reset(task); //shared_ptr初始化，指向task

    {
        std::lock_guard<std::mutex> guard(m_mutexList);       
        //m_taskList.push_back(std::make_shared<Task>(task));
        m_taskList.push_back(spTask); //Task对象入队
        std::cout << "Add a Task." << std::endl;
    }
    
    m_cv.notify_one(); //唤醒随机一个等待条件变量的线程
}

void TaskPool::removeAllTasks()   //析构时调用
{
    {
        std::lock_guard<std::mutex> guard(m_mutexList);
        for (auto& iter : m_taskList)
        {
            iter.reset();
        }
        m_taskList.clear();
    }
}