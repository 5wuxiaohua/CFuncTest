# 编译器
CC = gcc

# 编译器标志
CFLAGS += -g -O0 -Wall -Wextra -std=gnu99
# CFLAGS += Werror

# 定义目标文件和源文件的默认值
TARGET ?= main
SRC ?= $(TARGET).c

# 执行make时，带上TARGET参数，例如make TARGET=test
# 根据TARGET设置不同的LDFLAGS
ifeq ($(TARGET), dns_parse)
    LDFLAGS = -lldns -lpcap
else ifeq ($(TARGET), inotify)
    LDFLAGS = -lpthread -lpcap
else ifneq ($(findstring kafka, $(TARGET)),)
    LDFLAGS += -lrdkafka
else ifeq ($(TARGET), pcap_write_file)
    LDFLAGS = -lpcap
else ifeq ($(TARGET), thread_quit)
    LDFLAGS = -pthread
else ifeq ($(TARGET), cjasson_performance)
    LDFLAGS = -ljansson
else
    LDFLAGS = # 默认的 LDFLAGS
endif

# 默认目标
all: build/$(TARGET)

# 生成可执行文件
build/$(TARGET): $(SRC) Makefile | build
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

build:
	@mkdir -p $@

# 清理生成的文件
clean:
	rm -rf build
