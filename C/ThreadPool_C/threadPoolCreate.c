#include "threadpool.h"

// 创建线程池并初始化
ThreadPool *threadPoolCreate(int min, int max, int queueSize)
{
    // 为线程池分配内存
    ThreadPool *pool = (ThreadPool *)malloc(sizeof(ThreadPool));
    do
    {
        // 检查线程池内存分配是否成功
        if (pool == NULL)
        {
            printf("malloc threadpool fail...\n");
            break;
        }

        // 为线程ID数组分配内存，存储最大线程数的线程ID
        pool->threadIDs = (pthread_t *)malloc(sizeof(pthread_t) * max);
        if (pool->threadIDs == NULL)
        {
            printf("malloc threadIDs fail...\n");
            break;
        }
        // 初始化线程ID数组为0
        memset(pool->threadIDs, 0, sizeof(pthread_t) * max);

        // 初始化线程池的相关参数
        pool->minNum = min;  // 最小线程数
        pool->maxNum = max;  // 最大线程数
        pool->busyNum = 0;   // 当前忙碌的线程数
        pool->liveNum = min; // 当前存活的线程数，初始时与最小线程数相等
        pool->exitNum = 0;   // 需要退出的线程数，初始为0

        // 初始化线程池的互斥锁和条件变量
        if (pthread_mutex_init(&pool->mutexPool, NULL) != 0 ||
            pthread_mutex_init(&pool->mutexBusy, NULL) != 0 ||
            pthread_cond_init(&pool->notEmpty, NULL) != 0 ||
            pthread_cond_init(&pool->notFull, NULL) != 0)
        {
            printf("mutex or condition init fail...\n");
            break;
        }

        // 初始化任务队列
        pool->taskQ = (Task *)malloc(sizeof(Task) * queueSize);
        pool->queueCapacity = queueSize; // 队列的容量
        pool->queueSize = 0;             // 当前任务数为0
        pool->queueFront = 0;            // 队列头位置
        pool->queueRear = 0;             // 队列尾位置

        pool->shutdown = 0; // 初始化关闭标志为0，表示线程池开启状态

        // 创建管理者线程，负责动态增加或减少工作线程
        pthread_create(&pool->managerID, NULL, manager, pool);

        // 创建初始工作线程
        for (int i = 0; i < min; ++i)
        {
            pthread_create(&pool->threadIDs[i], NULL, worker, pool);
        }
        return pool; // 成功创建线程池，返回线程池指针

    } while (0);

    // 若创建失败，释放已分配的资源
    if (pool && pool->threadIDs)
        free(pool->threadIDs);
    if (pool && pool->taskQ)
        free(pool->taskQ);
    if (pool)
        free(pool);

    return NULL; // 返回 NULL表示创建失败
}
