#include "threadpool.h"

// 单个线程退出
void threadExit(ThreadPool *pool)
{
    // 获取当前线程的ID
    pthread_t tid = pthread_self();

    // 遍历线程池的线程ID数组，找到当前线程的ID位置
    for (int i = 0; i < pool->maxNum; ++i)
    {
        // 如果找到与当前线程ID匹配的ID
        if (pool->threadIDs[i] == tid)
        {
            // 将该位置的线程ID设置为0，表示线程已退出
            pool->threadIDs[i] = 0;
            printf("threadExit() called, %ld exiting...\n", tid);
            break;
        }
    }
    // 调用 pthread_exit()终止当前线程，并确保线程的资源被释放
    pthread_exit(NULL);
}
