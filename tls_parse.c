#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <pcap.h>
#include <mbedtls/ssl.h>

void packet_handler(u_char* user, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    // 获取 Ethernet 头
    struct ether_header* eth_header = (struct ether_header*)packet;

    // 获取 IP 头
    struct ip* ip_header = (struct ip*)(packet + sizeof(struct ether_header));

    // 获取 TCP 头
    struct tcphdr* tcp_header = (struct tcphdr*)(packet + sizeof(struct ether_header) + (ip_header->ip_hl << 2));

    // 计算 TCP 数据部分的起始位置
    const u_char* tls_payload = packet + sizeof(struct ether_header) + (ip_header->ip_hl << 2) + (tcp_header->th_off << 2);
    size_t tls_payload_length = pkthdr->len - (sizeof(struct ether_header) + (ip_header->ip_hl << 2) + (tcp_header->th_off << 2));

    // 检查是否是 TLS 协议的 Client Hello（通常在前5个字节中）
    if (tls_payload_length > 5 && tls_payload[0] == 0x16 && tls_payload[1] == 0x03) {
        // 初始化 mbed TLS SSL 上下文
        mbedtls_ssl_context ssl;
        mbedtls_ssl_init(&ssl);

        // 配置 mbed TLS SSL 上下文
        mbedtls_ssl_config conf;
        mbedtls_ssl_config_init(&conf);
        mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
        mbedtls_ssl_setup(&ssl, &conf);

        // 设置输入 TLS 数据包
        if (mbedtls_ssl_read_record(&ssl, MBEDTLS_SSL_MAJOR_VERSION_3, tls_payload, tls_payload_length) == 0) {
            // 解析 TLS 握手消息
            mbedtls_ssl_handshake(&ssl);

            // 获取服务器主机名（SNI）
            const char* server_name = mbedtls_ssl_conf_sni(&ssl);
            if (server_name) {
                printf("Server Name (SNI): %s\n", server_name);
            } else {
                printf("SNI not found\n");
            }

            // 清理资源
            mbedtls_ssl_free(&ssl);
        }

        mbedtls_ssl_config_free(&conf);
    }
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap = pcap_open_offline("pcap/tls.pcap", errbuf);

    if (pcap == NULL) {
        fprintf(stderr, "Error opening pcap file: %s\n", errbuf);
        return 1;
    }

    // Start capturing packets and call the packet_handler for each packet
    pcap_loop(pcap, 0, packet_handler, NULL);

    pcap_close(pcap);
    return 0;

    return 0;
}
