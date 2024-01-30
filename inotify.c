#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>
#include <pcap.h>

#define BUF_LEN (1024 * (sizeof(struct inotify_event) + 16))
#define PCAP_FILE_PATH "/opt/qt/http_server/pcap"

void packet_handler(uint8_t* user, const struct pcap_pkthdr* pkthdr, const uint8_t* packet) {
    printf("packet_handler\n");
}

void *process_pcap_file(void *filename) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap = pcap_open_offline(filename, errbuf);

    if (pcap == NULL) {
        fprintf(stderr, "Error opening pcap file: %s\n", errbuf);
        return 1;
    }

    // Start capturing packets and call the packet_handler for each packet
    pcap_loop(pcap, 0, packet_handler, NULL);

    pcap_close(pcap);

    pthread_exit(NULL);
}

int main() {
    int fd, wd;
    char buf[BUF_LEN];
    ssize_t numRead;
    pthread_t tid;
    
    fd = inotify_init();
    if (fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(fd, PCAP_FILE_PATH, IN_CLOSE_WRITE);
    if (wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    printf("Watching directory for new pcap files...\n");

    while (1) {
        numRead = read(fd, buf, BUF_LEN);
        if (numRead <= 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        printf("numRead:%d\n", numRead);

        for (char *p = buf; p < buf + numRead;) {
            struct inotify_event *event = (struct inotify_event *)p;
            if (event->mask & IN_CLOSE_WRITE) {
                if (!(event->mask & IN_ISDIR)) {
                    // 创建新线程处理 pcap 文件
                    char abs_path[128];
                    char *filename = strdup(event->name);
                    printf("filename:%s\n", filename);
                    sprintf(abs_path, "%s/%s", PCAP_FILE_PATH, filename);
                    printf("abs_path:%s\n", abs_path);
                    if (pthread_create(&tid, NULL, process_pcap_file, (void *)abs_path) != 0) {
                        fprintf(stderr, "Error creating thread for file: %s\n", abs_path);
                        free(filename);
                    }
                }
            }
            p += sizeof(struct inotify_event) + event->len;
        }
    }

    return 0;
}
