# 编译器
CC = gcc
# 编译器标志
CFLAGS += -g -O0 -Wall -Wextra -std=gnu99
# CFLAGS += -Werror
LDFLAGS += -lrdkafka
LDFLAGS += -lpthread
# 执行make时，带上TARGET参数，例如make TARGET=test
ifdef TARGET
    SRCS = $(TARGET).c
else
    SRCS = main.c
    TARGET = main
endif

# 目标文件
OBJS = $(SRCS:.c=.o)

# 默认目标
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 生成目标文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)
