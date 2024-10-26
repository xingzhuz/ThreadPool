# 线程池项目的 Makefile
# 定义变量
CC = gcc
CFLAGS = -Wall -g
SRC_DIR = ThreadPool_C
OBJ_DIR = obj
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) test.c
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# 目标可执行文件
TARGET = test

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@

# 编译源文件生成对象文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)  # 确保 obj 目录存在
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

.PHONY: all clean

