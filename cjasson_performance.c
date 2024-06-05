#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <jansson.h>
#include <time.h>

// 定义 IP 头部结构
struct ip_header {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ip_hl:4;      // header length
    unsigned int ip_v:4;       // version
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int ip_v:4;       // version
    unsigned int ip_hl:4;      // header length
#endif
    uint8_t ip_tos;            // type of service
    uint16_t ip_len;           // total length
    uint16_t ip_id;            // identification
    uint16_t ip_off;           // fragment offset field
    uint8_t ip_ttl;            // time to live
    uint8_t ip_p;              // protocol
    uint16_t ip_sum;           // checksum
    struct in_addr ip_src;     // source address
    struct in_addr ip_dst;     // dest address
};

// 生成 JSON 字符串使用 Jansson
char *generate_json_jansson(struct ip_header *ip) {
    json_t *root = json_object();
    json_object_set_new(root, "ip_v", json_integer(ip->ip_v));
    json_object_set_new(root, "ip_hl", json_integer(ip->ip_hl));
    json_object_set_new(root, "ip_tos", json_integer(ip->ip_tos));
    json_object_set_new(root, "ip_len", json_integer(ntohs(ip->ip_len)));
    json_object_set_new(root, "ip_id", json_integer(ntohs(ip->ip_id)));
    json_object_set_new(root, "ip_off", json_integer(ntohs(ip->ip_off)));
    json_object_set_new(root, "ip_ttl", json_integer(ip->ip_ttl));
    json_object_set_new(root, "ip_p", json_integer(ip->ip_p));
    json_object_set_new(root, "ip_sum", json_integer(ntohs(ip->ip_sum)));

    char src_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip->ip_src), src_ip, INET_ADDRSTRLEN);
    json_object_set_new(root, "src_ip", json_string(src_ip));

    char dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip->ip_dst), dst_ip, INET_ADDRSTRLEN);
    json_object_set_new(root, "dst_ip", json_string(dst_ip));

    char *json_str = json_dumps(root, JSON_ENCODE_ANY);
    json_decref(root);

    return json_str;
}

// 生成 JSON 字符串使用 snprintf
char *generate_json_snprintf(struct ip_header *ip) {
    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip->ip_src), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip->ip_dst), dst_ip, INET_ADDRSTRLEN);

    // 假设足够大的缓冲区来存储 JSON 字符串
    char *json_str = (char *)malloc(256);

    snprintf(json_str, 256,
             "{\"ip_v\":%d,\"ip_hl\":%d,\"ip_tos\":%d,\"ip_len\":%d,"
             "\"ip_id\":%d,\"ip_off\":%d,\"ip_ttl\":%d,\"ip_p\":%d,"
             "\"ip_sum\":%d,\"src_ip\":\"%s\",\"dst_ip\":\"%s\"}",
             ip->ip_v, ip->ip_hl, ip->ip_tos, ntohs(ip->ip_len),
             ntohs(ip->ip_id), ntohs(ip->ip_off), ip->ip_ttl, ip->ip_p,
             ntohs(ip->ip_sum), src_ip, dst_ip);

    return json_str;
}

int main() {
    // 示例 IP 数据包
    uint8_t packet[] = {
        0x45, 0x00, 0x00, 0x3c, 0x1c, 0x46, 0x40, 0x00,
        0x40, 0x06, 0xb1, 0xe6, 0xc0, 0xa8, 0x00, 0x68,
        0xc0, 0xa8, 0x00, 0x01
    };

    struct ip_header *ip = (struct ip_header *)packet;

    // 性能测试
    clock_t start, end;
    double cpu_time_used;
    char *json_str;

    // 测试 Jansson
    start = clock();
    for (int i = 0; i < 100000; i++) {
        json_str = generate_json_jansson(ip);
        free(json_str);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Jansson time: %f seconds\n", cpu_time_used);

    // 测试 snprintf
    start = clock();
    for (int i = 0; i < 100000; i++) {
        json_str = generate_json_snprintf(ip);
        free(json_str);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("snprintf time: %f seconds\n", cpu_time_used);

    return 0;
}
