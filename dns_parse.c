#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <pcap.h>
#include <ldns/ldns.h>

void process_dns_query(const uint8_t* dns_packet, size_t packet_length) {
    printf("process_dns_query:\n");
    // 创建 ldns_pkt 结构
    ldns_pkt* dns_query = ldns_pkt_new();

    // 解析 DNS 查询数据包
    if (ldns_wire2pkt(&dns_query, dns_packet, packet_length) != LDNS_STATUS_OK) {
        fprintf(stderr, "Failed to parse DNS query packet\n");
        ldns_pkt_free(dns_query);
        return;
    }

    // 获取查询字段并打印
    ldns_rr_list* question_list = ldns_pkt_question(dns_query);
    if (question_list) {
        ldns_rr* question = ldns_rr_list_rr(question_list, 0);
        if (question) {
            printf("DNS Query: %s\n", ldns_rdf2str(ldns_rr_owner(question)));
        }
    }

    // 释放 ldns_pkt 结构
    ldns_pkt_free(dns_query);
}

void process_dns_response(const uint8_t* dns_packet, size_t packet_length) {
    printf("process_dns_response:\n");
    // 创建 ldns_pkt 结构
    ldns_pkt* dns_response = ldns_pkt_new();

    // 解析 DNS 应答数据包
    if (ldns_wire2pkt(&dns_response, dns_packet, packet_length) != LDNS_STATUS_OK) {
        fprintf(stderr, "Failed to parse DNS response packet\n");
        ldns_pkt_free(dns_response);
        return;
    }

    // 获取应答部分并打印
    ldns_rr_list* answer_list = ldns_pkt_answer(dns_response);
    if (answer_list) {
        for (size_t i = 0; i < ldns_rr_list_rr_count(answer_list); ++i) {
            ldns_rr* answer = ldns_rr_list_rr(answer_list, i);
            if (answer) {
                // 获取查询名（QNAME）
                ldns_rdf* query_name = ldns_rr_owner(answer);
                printf("DNS Query Name: %s\n", ldns_rdf2str(query_name));

                // 获取应答中的地址记录
                if (ldns_rr_get_type(answer) == LDNS_RR_TYPE_A || ldns_rr_get_type(answer) == LDNS_RR_TYPE_AAAA) {
                    ldns_rdf* address_rdf = ldns_rr_rdf(answer, 0);
                    printf("DNS Answer Address: %s\n", ldns_rdf2str(address_rdf));
                }
            }
        }
    }

    // 释放 ldns_pkt 结构
    ldns_pkt_free(dns_response);
}


void packet_handler(u_char* user, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    // 在这里可以添加更多的数据包过滤逻辑
    // 这里简单示范，假设所有的数据包都是 IPv4 数据包
    struct iphdr* ip_header = (struct iphdr*)(packet + 14); // 14 是以太网帧头的长度

    // 检查是否是 IPv4 数据包且协议是 UDP（DNS 通常使用 UDP）
    if (ip_header->protocol == IPPROTO_UDP) {
        // 假设 UDP 头的长度为 8 字节
        const u_char* udp_payload = packet + 14 + (ip_header->ihl << 2) + 8;
        size_t udp_payload_length = pkthdr->len - (14 + (ip_header->ihl << 2) + 8);

        // 在这里检查是否是 DNS 查询数据包
        // 这里简单示范，你可能需要添加更多的逻辑以确保是 DNS 查询
        if (udp_payload_length > 12 && udp_payload[2] == 0x01 && udp_payload[3] == 0x00) {
            process_dns_query(udp_payload, udp_payload_length);
        }

        if (udp_payload_length > 12 && (udp_payload[2] & 0x80) == 0x80) {
            process_dns_response(udp_payload, udp_payload_length);
        }
    }
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap = pcap_open_offline("pcap/dns.pcap", errbuf);

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

