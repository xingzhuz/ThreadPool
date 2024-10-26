#include "threadpool.h"

// 销毁线程池
int threadPoolDestroy(ThreadPool *pool)
{
    // 如果线程池指针为空，直接返回错误代码 -1
    if (pool == NULL)
    {
        return -1;
    }

    // 关闭线程池，设置关闭标志位
    pool->shutdown = 1;

    // 阻塞等待管理者线程执行完毕
    pthread_join(pool->managerID, NULL);

    // 唤醒所有可能在等待任务的工作线程
    for (int i = 0; i < pool->liveNum; ++i)
    {
        pthread_cond_signal(&pool->notEmpty);
    }

    // 释放任务队列的内存
    if (pool->taskQ)
    {
        free(pool->taskQ);
    }

    // 释放线程ID数组的内存
    if (pool->threadIDs)
    {
        free(pool->threadIDs);
    }

    // 销毁互斥锁和条件变量
    pthread_mutex_destroy(&pool->mutexPool);
    pthread_mutex_destroy(&pool->mutexBusy);
    pthread_cond_destroy(&pool->notEmpty);
    pthread_cond_destroy(&pool->notFull);

    // 释放线程池结构体的内存
    free(pool);
    pool = NULL;

    return 0; // 返回0表示成功销毁线程池
}
