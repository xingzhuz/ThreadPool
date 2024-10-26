
## 目录结构

```
C/
├── Makefile            # 根目录的 Makefile
├── obj/                # 存放生成的对象文件(.o)
├── src/                # 线程池源文件的根目录
│   ├── threadpool.h
│   ├── threadPoolCreate.c
│   ├── threadPoolAdd.c      
│   ├── threadPoolDestroy.c      
│   ├── threadExit.c      
│   ├── manager.c
│   ├── worker.c     
│   └── getNum.c     
└── test.c            # 测试源文件
```


## 编译运行

```
# 生成可执行文件
make

# 运行测试代码
./test

# 清理可执行文件
make clean
```