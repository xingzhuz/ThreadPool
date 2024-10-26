#include "threadpool.h"

// 给线程池添加任务
void threadPoolAdd(ThreadPool *pool, void (*func)(void *), void *arg)
{
    // 加锁，保证线程池的任务队列操作是线程安全的
    pthread_mutex_lock(&pool->mutexPool);

    // 如果任务队列已满，并且线程池未关闭，生产者线程进入等待状态
    while (pool->queueSize == pool->queueCapacity && !pool->shutdown)
    {
        // 等待队列有空位时才继续添加任务
        pthread_cond_wait(&pool->notFull, &pool->mutexPool);
    }

    // 如果线程池已经关闭，直接解锁并退出
    if (pool->shutdown)
    {
        pthread_mutex_unlock(&pool->mutexPool);
        return;
    }

    // 将任务添加到任务队列的队尾位置
    pool->taskQ[pool->queueRear].function = func;                  // 设置任务的函数指针
    pool->taskQ[pool->queueRear].arg = arg;                        // 设置任务的参数
    pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity; // 更新队尾位置
    pool->queueSize++;                                             // 队列中的任务数+1

    // 发送信号通知有任务可以消费
    pthread_cond_signal(&pool->notEmpty);

    // 解锁，允许其他线程访问任务队列
    pthread_mutex_unlock(&pool->mutexPool);
}
