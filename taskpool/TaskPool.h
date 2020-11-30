/** 
 * 任务池模型，TaskPool.h
 */

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <vector>
#include <memory>
#include <iostream>
#include <functional> //for visual studio build

class Task
{
public:
    virtual void doIt()
    {
        std::cout << "Do a task..." << std::endl;
    }

    virtual ~Task()
    {
        //为了看到一个task的销毁，这里刻意补上其析构函数
        std::cout << "A task destructed..." << std::endl;
    }
};

class TaskPool final
{
public:
    TaskPool();
    ~TaskPool();
    TaskPool(const TaskPool& rhs) = delete;     //delete: 禁用某函数（C++11），阻止拷贝和赋值构造，C++98用private
    TaskPool& operator=(const TaskPool& rhs) = delete;

public:
    void init(int threadNum = 5);   //默认初始化
    void stop();

    void addTask(Task* task);
    void removeAllTasks();

private:
    void threadFunc();

private:
    std::list<std::shared_ptr<Task>>            m_taskList;
    std::mutex                                  m_mutexList;
    std::condition_variable                     m_cv;
    bool                                        m_bRunning;
    std::vector<std::shared_ptr<std::thread>>   m_threads;
};