/**
 * �����ģ�ͣ�TaskPool.cpp
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
        //shared_ptr.reset�������ǳ�ʼ����ָ��new����thread����
        //bind����thread�������ִ�к���threadFunc
        spThread.reset(new std::thread(std::bind(&TaskPool::threadFunc, this))); 
        m_threads.push_back(spThread); //thread�������
    }
}

void TaskPool::threadFunc() //thread������ʱִ��
{
    std::shared_ptr<Task> spTask;
    while (true)
    {
        std::unique_lock<std::mutex> guard(m_mutexList); //RAIIʵ�֣�����������Զ�����
        while (m_taskList.empty())
        {                 
            if (!m_bRunning)
                break;
            
            //�������˻��������������������ʵĻ���pthread_cond_wait���ͷ�����������ִ�С�
            //�������仯���������ʣ�pthread_cond_wait��ֱ�ӻ������
            m_cv.wait(guard);
        }

        if (!m_bRunning)
            break;

        spTask = m_taskList.front(); //ȡm_taskList��task����
        m_taskList.pop_front(); //����m_taskList

        if (spTask == NULL)
            continue;

        spTask->doIt(); //ִ��task
        spTask.reset(); //shared_ptr.reset����������ָ�����ļ���-1
    }

    std::unique_lock<std::mutex> guard(m_mutexList); //Ϊ�˴�ӡ��ԭ���ԣ��ټ���
    {
        std::cout << "Exit thread, threadID: " << std::this_thread::get_id() << std::endl;
    }
    
}

void TaskPool::stop()
{
    m_bRunning = false;
    m_cv.notify_all(); //�������еȴ������������߳�

    //�ȴ������߳��˳�
    for (auto& iter : m_threads)
    {
        if (iter->joinable())   //���߳��Ƿ��join
            iter->join();       //���̵߳ȴ����߳�
    }
}

void TaskPool::addTask(Task* task)
{
    std::shared_ptr<Task> spTask;
    spTask.reset(task); //shared_ptr��ʼ����ָ��task

    {
        std::lock_guard<std::mutex> guard(m_mutexList);       
        //m_taskList.push_back(std::make_shared<Task>(task));
        m_taskList.push_back(spTask); //Task�������
        std::cout << "Add a Task." << std::endl;
    }
    
    m_cv.notify_one(); //�������һ���ȴ������������߳�
}

void TaskPool::removeAllTasks()   //����ʱ����
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