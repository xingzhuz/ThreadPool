#include "threadpool.h"

// 管理者线程处理动作
void *manager(void *arg)
{
    // 将参数转换为线程池结构
    ThreadPool *pool = (ThreadPool *)arg;

    // 循环运行，直到线程池关闭
    while (!pool->shutdown)
    {
        // 管理者线程每3秒检测一次线程池状态
        sleep(3);

        // 取出线程池中当前任务数量和存活的线程数量
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->queueSize; // 当前任务队列中的任务数
        int liveNum = pool->liveNum;     // 当前存活的线程数
        pthread_mutex_unlock(&pool->mutexPool);

        // 获取当前忙碌线程的数量
        pthread_mutex_lock(&pool->mutexBusy);
        int busyNum = pool->busyNum; // 正在执行任务的线程数
        pthread_mutex_unlock(&pool->mutexBusy);

        // 添加新线程的逻辑 (自定义的)
        // 条件：任务数 > 存活线程数，且当前线程数 < 最大线程数
        if (queueSize > liveNum && liveNum < pool->maxNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            int counter = 0; // 计数器，记录本次要添加的线程数

            // 循环创建线程，直到达到配置的最大线程数或指定添加的线程数
            for (int i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; ++i)
            {
                // 仅在当前线程ID为空的情况下创建新线程
                if (pool->threadIDs[i] == 0)
                {
                    pthread_create(&pool->threadIDs[i], NULL, worker, pool); // 创建工作线程
                    counter++;                                               // 增加计数器，表示添加的线程数量
                    pool->liveNum++;                                         // 增加线程池的总存活线程数
                }
            }
            pthread_mutex_unlock(&pool->mutexPool);
        }

        // 销毁多余线程的逻辑
        // 条件：忙碌线程数量 * 2 < 存活线程数 且 存活线程数 > 最小线程数
        if (busyNum * 2 < liveNum && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            pool->exitNum = NUMBER; // 设置 exitNum，表示本轮销毁的线程数量
            pthread_mutex_unlock(&pool->mutexPool);

            // 向工作线程发出信号，使其自我销毁
            for (int i = 0; i < NUMBER; ++i)
            {
                pthread_cond_signal(&pool->notEmpty); // 发送信号通知空闲线程结束
            }
        }
    }
    return NULL;
}
