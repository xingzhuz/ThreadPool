# Makefile

# 编译器
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I. 

# 目录
BIN_DIR = bin
OBJ_DIR = obj
TEST_DIR = test
THREADPOOL_SYNC_DIR = ThreadPool_sync
THREADPOOL_ASYNC_DIR = ThreadPool_async

# 源文件和目标文件
SYNC_TEST_SRC = $(TEST_DIR)/test_sync.cpp
ASYNC_TEST_SRC = $(TEST_DIR)/test_async.cpp
SYNC_OBJ = $(OBJ_DIR)/threadpool_sync.o
ASYNC_OBJ = $(OBJ_DIR)/threadpool_async.o

SYNC_EXEC = $(BIN_DIR)/test_sync
ASYNC_EXEC = $(BIN_DIR)/test_async

# 规则
all: $(OBJ_DIR) $(BIN_DIR) $(SYNC_EXEC) $(ASYNC_EXEC)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(SYNC_EXEC): $(SYNC_TEST_SRC) $(SYNC_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(ASYNC_EXEC): $(ASYNC_TEST_SRC) $(ASYNC_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 生成同步线程池的对象文件
$(OBJ_DIR)/threadpool_sync.o: $(THREADPOOL_SYNC_DIR)/threadpool_sync.cpp $(THREADPOOL_SYNC_DIR)/threadpool_sync.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 生成异步线程池的对象文件
$(OBJ_DIR)/threadpool_async.o: $(THREADPOOL_ASYNC_DIR)/threadpool_async.cpp $(THREADPOOL_ASYNC_DIR)/threadpool_async.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*

.PHONY: all clean
