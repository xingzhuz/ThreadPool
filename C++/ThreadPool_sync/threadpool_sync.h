#include <thread>
#include <map>
#include <queue>
#include <functional>
#include <atomic>
#include <future>
#include <condition_variable>
#include <vector>
#include <mutex>
using namespace std;

class ThreadPool
{
public:
    ThreadPool(int minn = 3, int maxn = thread::hardware_concurrency());
    ~ThreadPool();

    void addTask(function<void(void)> f);

private:
    void worker();
    void manager();

private:
    // 工作的线程
    // 这里定义为指针，是因为后续创建线程直接 new 出来即可，因为线程之间是不允许拷贝构造的
    // 如果写 thread m_manager; 后续不能直接使用 = 赋值，除非使用移动构造转移给 m_manager
    thread *m_manager;

    // 工作的线程
    map<thread::id, thread> m_workers;

    // 存储已经退出了的线程 id，主要用于销毁线程
    vector<thread::id> m_ids;

    // 任务队列
    queue<function<void(void)>> m_tasks;

    // 最小最大线程数
    int m_minThreads, m_maxThreads;

    // 空闲线程数，当前线程数
    atomic<int> m_idleThreads, m_curThreads;

    // 一次性退出的线程数
    atomic<int> m_exitNum;

    // 线程池是否关闭
    atomic<bool> m_stop;

    // 互斥量
    mutex m_idMutex;
    mutex m_queueMutex;

    // 条件变量
    condition_variable m_notEmpty;
};