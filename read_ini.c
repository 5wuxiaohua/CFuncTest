#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char brokers[128];
    char topic[32];
    char batch_num_messages[16];
    char queue_buffering_max_ms[16];
    char queue_buffering_max_kbytes[16];
    char queue_buffering_max_messages[16];
    char request_timeout_ms[16];
    char message_send_max_retries[16];
    char message_copy_max_bytes[16];
    char socket_send_buffer_bytes[16];
    char compression_codec[16];
} kafka_config;

kafka_config kafka_config_data = {0};

void trim_whitespace(char* str) {
    int len = strlen(str);
    int i, j, k;

    // 从前往后找到第一个非空白字符
    for (i = 0; i < len; i++) {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r') {
            break;
        }
    }

    // 从后往前找到第一个非空白字符
    for (j = len - 1; j >= 0; j--) {
        if (str[j] != ' ' && str[j] != '\t' && str[j] != '\n' && str[j] != '\r') {
            break;
        }
    }

    // 移动非空白字符到前面
    for (k = i; k <= j; k++) {
        str[k - i] = str[k];
    }

    // 在最后一个非空白字符之后加上结束符
    str[j - i + 1] = '\0';
}

void read_ini_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file: %s\n", filename);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        printf("line: %s\n", line);
        // 去除行首和行尾的空白字符
        trim_whitespace(line);

        if (line[0] == ';' || line[0] == '#' || line[0] == '\0') {
            // 跳过注释行和空行
            continue;
        }

        char* key = strtok(line, "=");
        char* value = strtok(NULL, "=");
        if (key == NULL || value == NULL) {
            printf("=======\n");
        }
        // 去除键和值中的空白字符
        trim_whitespace(key);
        trim_whitespace(value);

        if (key && value) {
            if (strcmp(key, "broker") == 0) {
                strcpy(kafka_config_data.brokers, value);
            } else if (strcmp(key, "topic") == 0) {
                strcpy(kafka_config_data.topic, value);
            } else if (strcmp(key, "batch.num.messages") == 0) {
                strcpy(kafka_config_data.batch_num_messages, value);
            } else if (strcmp(key, "queue.buffering.max.ms") == 0) {
                strcpy(kafka_config_data.queue_buffering_max_ms, value);
            } else if (strcmp(key, "queue.buffering.max.kbytes") == 0) {
                strcpy(kafka_config_data.queue_buffering_max_kbytes, value);
            } else if (strcmp(key, "queue.buffering.max.messages") == 0) {
                strcpy(kafka_config_data.queue_buffering_max_messages, value);
            } else if (strcmp(key, "request.timeout.ms") == 0) {
                strcpy(kafka_config_data.request_timeout_ms, value);
            } else if (strcmp(key, "message.send.max.retries") == 0) {
                strcpy(kafka_config_data.message_send_max_retries, value);
            } else if (strcmp(key, "message.copy.max.bytes") == 0) {
                strcpy(kafka_config_data.message_copy_max_bytes, value);
            } else if (strcmp(key, "socket.send.buffer.bytes") == 0) {
                strcpy(kafka_config_data.socket_send_buffer_bytes, value);
            } else if (strcmp(key, "compression.codec") == 0) {
                strcpy(kafka_config_data.compression_codec, value);
            } else {
                continue;
            }
        }
    }

    fclose(file);
}

int main() {
    char *filename = "test.ini";
    read_ini_file(filename);

    printf("brokers:%s\n", kafka_config_data.brokers);
    printf("topic:%s\n", kafka_config_data.topic);
    printf("batch_num_messages:%s\n", kafka_config_data.batch_num_messages);
    printf("queue_buffering_max_ms:%s\n", kafka_config_data.queue_buffering_max_ms);
    printf("queue_buffering_max_messages:%s\n", kafka_config_data.queue_buffering_max_messages);
    printf("request_timeout_ms:%s\n", kafka_config_data.request_timeout_ms);
    printf("message_send_max_retries:%s\n", kafka_config_data.message_send_max_retries);
    printf("message_copy_max_bytes:%s\n", kafka_config_data.message_copy_max_bytes);
    printf("socket_send_buffer_bytes:%s\n", kafka_config_data.socket_send_buffer_bytes);
    printf("compression_codec:%s\n", kafka_config_data.compression_codec);
  
    return 0;
}