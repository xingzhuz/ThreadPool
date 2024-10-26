#include "threadpool.h"

// 获取线程池中正在执行任务的线程数
int threadPoolBusyNum(ThreadPool *pool)
{
    // 锁定 busyNum的互斥锁，确保读取过程中数据一致性
    pthread_mutex_lock(&pool->mutexBusy);
    int busyNum = pool->busyNum;            // 获取当前忙碌线程数
    pthread_mutex_unlock(&pool->mutexBusy); // 解锁互斥锁
    return busyNum;                         // 返回忙碌线程数
}

// 获取线程池中存活的线程数
int threadPoolAliveNum(ThreadPool *pool)
{
    // 锁定 liveNum的互斥锁，确保读取过程中数据一致性
    pthread_mutex_lock(&pool->mutexPool);
    int aliveNum = pool->liveNum;           // 获取当前存活线程数
    pthread_mutex_unlock(&pool->mutexPool); // 解锁互斥锁
    return aliveNum;                        // 返回存活线程数
}
