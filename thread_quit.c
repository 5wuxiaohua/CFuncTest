#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <locale.h>
#define __USE_GNU
#include <sched.h>
#include <pthread.h>

// 用于通知线程退出的标志
volatile sig_atomic_t exit_flag = 0;

// 线程函数，重复打印消息
void *printHello(void *arg) {
    // 绑定线程到 CPU 1
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        perror("pthread_setaffinity_np");
        exit(EXIT_FAILURE);
    }

    while (!exit_flag) {
        // printf("Hello\n");
        // sleep(1);  // 等待1秒
    }
    pthread_exit(NULL);
}

// SIGINT 信号处理函数
void sigintHandler(int signum) {
    exit_flag = 1;
}

int main() {
    // 注册 SIGINT 信号处理函数
    signal(SIGINT, sigintHandler);
    for (int i = 0; i < 10; i++);
    pthread_t tid;
    // 创建子线程
    if (pthread_create(&tid, NULL, printHello, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // 绑定主线程到 CPU 0
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        perror("pthread_setaffinity_np");
        exit(EXIT_FAILURE);
    }

    while (!exit_flag) {
        // printf("main Hello\n");
        // sleep(1);  // 等待1秒
    }

    // 等待子线程结束
    if (pthread_join(tid, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    printf("All threads exited.\n");

    return 0;
}
