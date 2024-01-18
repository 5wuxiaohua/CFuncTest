# 编译器
CC = gcc

# 编译器标志
CFLAGS += -g -O0 -Wall -Wextra -std=gnu99

# CFLAGS += -Werror
LDFLAGS += -lrdkafka
LDFLAGS += -lpthread
LDFLAGS += -lcares
LDFLAGS += -lpcap
LDFLAGS += -lldns

# 执行make时，带上TARGET参数，例如make TARGET=test
ifdef TARGET
    SRCS = $(TARGET).c
else
    SRCS = main.c
    TARGET = main
endif

# 默认目标
all: build/$(TARGET)

# 生成可执行文件
build/$(TARGET): $(SRCS) Makefile | build
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS)

build:
	@mkdir -p $@

# 清理生成的文件
clean:
	rm -rf build
