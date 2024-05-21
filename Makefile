CC = g++

CFLAGS = -Wall -g -std=c++11

SOURCES = microDB.cpp

# 指定生成的目标（可执行文件）
EXECUTABLE = microDB

all: $(EXECUTABLE)

$(EXECUTABLE):$(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@

# 清理目标，删除生成的文件
clean:
	rm -f $(EXECUTABLE)

# make 命令默认执行 all 目标
.DEFAULT_GOAL := all
