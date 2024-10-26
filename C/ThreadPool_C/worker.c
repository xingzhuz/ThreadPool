#include "threadpool.h"

// 工作的线程处理动作
void *worker(void *arg)
{
    // 将参数转换为线程池类型
    ThreadPool *pool = (ThreadPool *)arg;

    while (1)
    {
        // 加锁，保护线程池任务队列的访问
        pthread_mutex_lock(&pool->mutexPool);

        // 当任务队列为空且线程池未关闭时，工作线程等待新任务
        while (pool->queueSize == 0 && !pool->shutdown)
        {
            // 阻塞工作线程，等待条件变量 `notEmpty` 被信号唤醒，表示任务队列不为空
            pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);

            // 检查是否需要销毁多余的空闲线程
            if (pool->exitNum > 0)
            {
                pool->exitNum--;                  // 递减退出线程数
                if (pool->liveNum > pool->minNum) // 保持线程数量不少于最小存活线程数
                {
                    pool->liveNum--;                        // 减少线程数量
                    pthread_mutex_unlock(&pool->mutexPool); // 解锁，退出临界区
                    threadExit(pool);                       // 调用退出线程函数，结束当前线程
                }
            }
        }

        // 再次检查线程池是否关闭
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&pool->mutexPool); // 解锁，避免死锁
            threadExit(pool);                       // 退出线程
        }

        // 从任务队列中取出一个任务
        Task task;
        task.function = pool->taskQ[pool->queueFront].function; // 任务的执行函数
        task.arg = pool->taskQ[pool->queueFront].arg;           // 任务参数

        // 移动队列头指针，指向下一个任务位置
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity; // 这里采用循环队列方式处理
        pool->queueSize--;                                               // 减少队列中任务的数量

        // 唤醒可能等待任务队列的生产者线程，表明队列中有空位
        pthread_cond_signal(&pool->notFull);

        // 解锁任务队列互斥锁
        pthread_mutex_unlock(&pool->mutexPool);

        // 输出日志，表示线程开始工作
        printf("thread %ld start working...\n", pthread_self());

        // 加锁以修改 `busyNum`，统计当前繁忙线程数量
        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum++; // 增加忙碌线程计数
        pthread_mutex_unlock(&pool->mutexBusy);

        // 执行任务函数并传递参数
        task.function(task.arg);
        free(task.arg);  // 释放任务参数的内存
        task.arg = NULL; // 清空指针

        // 输出日志，表示线程完成工作
        printf("thread %ld end working...\n", pthread_self());

        // 任务完成后，减少 `busyNum` 计数
        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum--; // 减少忙碌线程计数
        pthread_mutex_unlock(&pool->mutexBusy);
    }
    return NULL;
}
