// 利用linux原生socket和sendto接口构造数据包，构造的数据包通过linux路由发送，所以srcip和dstip必须是实际的可路由的

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>  // for sockaddr_in, inet_pton
#include <sys/socket.h>  // for socket, sendto

#define DEST_IP "172.20.15.2"  // 目标 IP 地址
#define DEST_PORT 12345           // 目标端口
#define SRC_IP "172.20.15.1"    // 指定的网络接口 IP 地址（eno1 的 IP）

int main() {
    int sockfd;
    struct sockaddr_in dest_addr, src_addr;
    char *message = "Hello, this is a test message";  // 要发送的消息
    ssize_t bytes_sent;

    // 创建一个 UDP 套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置源地址（绑定到指定 IP 地址）
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = 0;  // 让操作系统选择源端口
    if (inet_pton(AF_INET, SRC_IP, &src_addr.sin_addr) <= 0) {
        perror("Invalid source IP address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 绑定套接字到源 IP 地址（指定接口）
    if (bind(sockfd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 填充目标地址结构
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);  // 目标端口
    if (inet_pton(AF_INET, DEST_IP, &dest_addr.sin_addr) <= 0) {
        perror("Invalid destination address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 发送数据包
    bytes_sent = sendto(sockfd, message, strlen(message), 0, 
                        (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (bytes_sent < 0) {
        perror("sendto failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Sent %zd bytes to %s:%d via %s\n", bytes_sent, DEST_IP, DEST_PORT, SRC_IP);

    // 关闭套接字
    close(sockfd);
    return 0;
}
