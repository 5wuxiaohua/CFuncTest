#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <librdkafka/rdkafka.h>

#define BATCH_SIZE 10000

int main(int argc, char *argv[]) {
    int num_messages = atoi(argv[1]);
    // Kafka配置
    rd_kafka_conf_t *conf;
    rd_kafka_conf_res_t conf_res;

    // 创建Kafka配置
    conf = rd_kafka_conf_new();

    rd_kafka_conf_set(conf, "batch.num.messages", "1000000", NULL, 0);
    rd_kafka_conf_set(conf, "queue.buffering.max.ms", "1000", NULL, 0);
    rd_kafka_conf_set(conf, "queue.buffering.max.kbytes", "5120000", NULL, 0);
    rd_kafka_conf_set(conf, "queue.buffering.max.messages", "10000000", NULL, 0);

    // 设置Kafka代理地址，根据你的实际情况修改
    const char *brokers = "192.168.1.131:19092,192.168.1.132:19092,192.168.1.133:19092";
    conf_res = rd_kafka_conf_set(conf, "bootstrap.servers", brokers, NULL, 0);
    if (conf_res != RD_KAFKA_CONF_OK) {
        fprintf(stderr, "Error setting bootstrap.servers: %s\n", rd_kafka_err2str(conf_res));
        return 1;
    }

    // 创建Kafka生产者
    rd_kafka_t *rk;
    rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, NULL, 0);
    if (!rk) {
        fprintf(stderr, "Error creating producer\n");
        return 1;
    }

    // Kafka主题配置
    rd_kafka_topic_conf_t *topic_conf;
    topic_conf = rd_kafka_topic_conf_new();

    // 创建Kafka主题
    const char *topic_name = "test-topic9";  // 根据你的实际情况修改
    rd_kafka_topic_t *rkt;
    rkt = rd_kafka_topic_new(rk, topic_name, topic_conf);
    if (!rkt) {
        fprintf(stderr, "Error creating topic object\n");
        return 1;
    }

    // 生产消息
    const char *message_payload = "Hello, Kafka!";  // 根据你的实际情况修改
    size_t len = strlen(message_payload);
    rd_kafka_resp_err_t err;

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    
    // 发送消息
    for (int i = 0; i < num_messages; ++i) {
        err = rd_kafka_produce(
            rkt,
            RD_KAFKA_PARTITION_UA,
            RD_KAFKA_MSG_F_COPY,
            (void *)message_payload,
            len,
            NULL,
            0,
            NULL
        );
        if (err) {
            fprintf(stderr, "Failed to produce message: %s\n", rd_kafka_err2str(err));
        }

        // 批量发送
        // if ((i + 1) % BATCH_SIZE == 0) {
        //     // 等待所有消息发送完成
        //     rd_kafka_flush(rk, 10000);  // 等待最多10秒，确保消息发送完成
        // }
    }

    // 等待所有消息发送完成
    rd_kafka_flush(rk, 10000);  // 等待最多10秒，确保消息发送完成

    // 获取结束时间
    gettimeofday(&end_time, NULL);

    // 计算用时
    long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000 +
                        (end_time.tv_usec - start_time.tv_usec);

    // 打印结果
    fprintf(stdout, "Messages sent: %d\n", num_messages);
    fprintf(stdout, "Elapsed time: %ld microseconds\n", elapsed_time);

    // 销毁Kafka生产者和相关资源
    rd_kafka_topic_destroy(rkt);
    rd_kafka_destroy(rk);

    return 0;
}
