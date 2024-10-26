
## 目录结构

```
C++/
├── Makefile                        # 根目录的 Makefile
├── bin/                            # 存放编译生成的可执行文件
│   ├── test_sync                   # 同步线程池测试可执行文件
│   └── test_async                  # 异步线程池测试可执行文件
├── obj/                            # 存放生成的对象文件
│   ├── threadpool_sync.o           # 同步线程池的对象文件
│   └── threadpool_async.o          # 异步线程池的对象文件
├── ThreadPool_sync/                # 同步线程池源文件的根目录
│   ├── threadpool_sync.h           # 同步线程池头文件
│   └── threadpool_sync.cpp         # 同步线程池实现文件
├── ThreadPool_async/               # 异步线程池源文件的根目录
│   ├── threadpool_async.h          # 异步线程池头文件
│   ├── threadpool_async.cpp        # 异步线程池实现文件
│   └── README.md                   # 异步线程池的说明文件
└── test/                           # 测试源文件目录
    ├── test_sync.cpp               # 同步线程池的测试文件
    └── test_async.cpp              # 异步线程池的测试文件
```



## 简介

- `ThreadPool_sync` 是使用`C++11`新特性实现的同步线程池
- `ThreadPool_async` 是使用`C++11`新特性实现的异步步线程池
- 同步线程池是需要阻塞等待结果，等待执行，然后得到执行结果
- 异步线程池实现了不需要阻塞等待，发起任务后，做其他事，发起的任务执行完毕后，返回执行结果给主线程




## 编译运行

```
# 编译
make

# 运行
./bin/test_sync   # 运行同步线程池测试文件
./bin/test_async  # 运行异步线程池测试文件

# 清除所有可执行文件
make clean
```
