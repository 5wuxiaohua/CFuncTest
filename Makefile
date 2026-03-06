# 编译器
CC = gcc

# 编译选项
CFLAGS += -g -O0 -Wall -Wextra -std=gnu99
LDFLAGS += -lldns -lpcap -lpthread -lrdkafka -ljansson

# 可执行文件输出目录
BUILD_DIR = build

# 默认目标：帮助信息
.PHONY: all
all:
	@echo "Usage: make <target>  # e.g., make a"

%:
	@$(MAKE) $(BUILD_DIR)/$@
# 通用规则：根据目标名编译对应的 .c 文件
$(BUILD_DIR)/%: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 创建 build 目录（如果不存在）
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# clean 目标：删除 build 目录及所有可执行文件
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)