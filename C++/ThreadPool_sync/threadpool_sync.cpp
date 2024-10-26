#include "threadpool_sync.h"
#include <iostream>

// 初始化默认构造函数
ThreadPool::ThreadPool(int minn, int maxn)
    : m_minThreads(minn), // 初始化最小线程数
      m_maxThreads(maxn), // 初始化最大线程数
      m_exitNum(0),       // 初始化退出线程计数为0
      m_stop(false)       // 初始化停止标志为false
{
    m_idleThreads = m_curThreads = minn; // 当前线程数和空闲线程数都设置为最小线程数

    // 启动管理线程，该线程负责管理线程池中的线程
    m_manager = new thread(&ThreadPool::manager, this);

    // 根据最小线程数初始化创建工作线程
    for (int i = 0; i < m_curThreads; i++)
    {
        // 创建一个工作线程并将其与线程池关联
        thread t(&ThreadPool::worker, this);
        // 将工作线程的ID和线程对象存入 m_workers 中
        m_workers.insert(make_pair(t.get_id(), std::move(t)));
    }
}

ThreadPool::~ThreadPool()
{
    m_stop.store(true); // 设置停止标志，通知所有工作线程停止工作

    m_notEmpty.notify_all(); // 唤醒所有等待任务的线程，以便它们可以检查停止标志

    cout << "线程池销毁中....." << endl; // 输出销毁过程的提示信息

    // 遍历所有工作线程
    for (auto &it : m_workers)
    {
        // 使用引用来避免线程对象的拷贝
        thread &t = it.second;
        if (t.joinable()) // 检查线程是否可以被连接
        {
            cout << "----------- 线程 " << t.get_id() << "最终被销毁" << endl; // 输出线程销毁信息
            t.join();                                                          // 等待线程完成执行
        }
    }

    if (m_manager->joinable()) // 检查管理线程是否可以被连接
    {
        m_manager->join(); // 等待管理线程完成执行
    }

    // 删除管理线程对象以释放堆内存
    delete m_manager;
}

// 增加任务函数
void ThreadPool::addTask(function<void(void)> f)
{
    {
        // 创建一个唯一锁，锁住任务队列以保证线程安全
        unique_lock<mutex> locker(m_queueMutex);

        // 将任务添加到任务队列中
        m_tasks.emplace(f);
    } // 释放锁，退出作用域后自动解锁

    // 通知一个等待的工作线程有新任务可处理
    m_notEmpty.notify_one();
}

// 实现工作的线程函数
void ThreadPool::worker()
{
    // 当线程池没有关闭的时候
    while (!m_stop.load())
    {
        function<void(void)> task = nullptr;

        {
            // 这个大括号是局部作用域，表示锁的范围，只需要锁住取任务，任务的执行不需要加锁
            unique_lock<mutex> locker(m_queueMutex);

            // 任务队列为空
            // 注意这里一定要使用 while，而不是 if，不然会存在 bug
            // 假如唤醒了多个阻塞在这里的线程，如果为 if，全部都会抢互斥锁，
            // 然后直接向下执行，而不会判断是否还有任务没被抢。
            // 但是可能此时任务队列中只有一个任务，就会造成非法访问。
            // 如果为 while，抢到锁后也需要先判断条件是否满足，才能退出循环向下执行，这样就不存在上述的问题了。
            while (!m_stop.load() && m_tasks.empty())
            {
                // 等待通知，释放锁并阻塞当前线程，直到有任务可用
                m_notEmpty.wait(locker);

                // 检查是否需要退出线程
                if (m_exitNum.load() > 0)
                {
                    m_curThreads--; // 当前线程数减一
                    m_exitNum--;    // 退出线程数减一
                    cout << "--------------线程退出了----------" << endl;

                    // 共享资源加锁
                    unique_lock<mutex> locker1(m_idMutex);
                    // 记录当前线程的ID
                    m_ids.emplace_back(this_thread::get_id());
                    return; // 退出当前线程
                }
            }

            // 取出任务
            cout << "取出了一个任务..." << endl;
            task = m_tasks.front(); // 获取队列头部的任务
            m_tasks.pop();          // 从队列中移除该任务
        } // 释放锁

        // 空闲线程数变化
        m_idleThreads--;
        task();          // 执行任务
        m_idleThreads++; // 完成任务后，增加空闲线程数
    }
}

// 管理者线程的处理动作
void ThreadPool::manager()
{
    while (!m_stop.load())
    {
        // 3 秒进行检测一次
        this_thread::sleep_for(chrono::seconds(3));

        int idel = m_idleThreads.load();
        int cur = m_curThreads.load();

        // 销毁线程(自定义规则)
        // 这里定义为: 空闲线程数 > 当前线程数 / 2 && 当前线程数 - 2 >= 最小线程数
        // 减去 2 是因为我定义的是一次性销毁 2 个线程数
        if (idel > cur / 2 && cur - 2 >= m_minThreads)
        {
            // 销毁线程不是管理者线程销毁，而是唤醒工作的线程，让其自己退出，管理者线程可以用于回收退出的线程资源
            // 一次性销毁 2 个
            m_exitNum.store(2);
            m_notEmpty.notify_all(); // 虽然是唤醒了所有的线程，但是只会有 2 个线程销毁
            // 这个 m_ids 容器是共享资源，因此需要加锁
            unique_lock<mutex> locker(m_idMutex);
            for (const auto &id : m_ids)
            {
                auto it = m_workers.find(id);
                if (it != m_workers.end())
                {
                    cout << "------------ 线程 " << (*it).first << "被销毁了...." << endl;

                    // 这里看似会阻塞程序运行，实则不会，因为这些线程都在 worker 函数中退出了，这个只是回收 pcb 等资源
                    (*it).second.join();
                    m_workers.erase(it);
                }
            }
            m_ids.clear();
        }

        // 添加线程
        // 自定义: 空闲的线程数为 0 && 当前线程数 + 2 <= 最大线程数
        if (idel == 0 && cur + 2 <= m_maxThreads)
        {
            thread t(&ThreadPool::worker, this);
            cout << "++++++ 添加了一个线程, id: " << t.get_id() << endl;
            m_workers.insert(make_pair(t.get_id(), std::move(t)));
            m_curThreads++, m_idleThreads++;
        }
    }
}