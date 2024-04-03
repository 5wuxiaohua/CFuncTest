#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <sys/types.h>

#define SNAP_LEN 1518
#define PROMISCUOUS 1
#define TIMEOUT_MS 1000

void packet_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet);

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    pcap_dumper_t *dumper;

    // 打开网卡 eth0
    handle = pcap_open_live("eth0", SNAP_LEN, PROMISCUOUS, TIMEOUT_MS, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Error opening device: %s\n", errbuf);
        return EXIT_FAILURE;
    }

    // 打开 pcap 文件用于写入数据包
    dumper = pcap_dump_open(handle, "123.pcap");
    if (dumper == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        return EXIT_FAILURE;
    }

    // 开始捕获数据包并写入文件
    pcap_loop(handle, -1, packet_handler, (u_char *)dumper);

    // 关闭 pcap 文件
    pcap_dump_close(dumper);

    // 关闭网卡
    pcap_close(handle);

    return EXIT_SUCCESS;
}

void packet_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    pcap_dump(user, pkthdr, packet);
}
