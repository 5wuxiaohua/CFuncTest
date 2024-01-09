#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <librdkafka/rdkafka.h>

#define BATCH_SIZE 10000
#define NUM_MESSAGES 1000000
#define NUM_THREADS 4  // 设置线程数量

static int num_messages_sum = 0;

void produce_messages(rd_kafka_t *rk, rd_kafka_topic_t *rkt, const char *message_payload, size_t len, int num_messages) {
    rd_kafka_resp_err_t err;

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
            break;
        }
    }

    // 确保所有消息发送完成
    rd_kafka_flush(rk, 10000);
}

void *thread_function(void *arg) {
    int thread_id = *((int *)arg);

    // Kafka配置
    rd_kafka_conf_t *conf;
    rd_kafka_conf_res_t conf_res;

    // 创建Kafka配置
    conf = rd_kafka_conf_new();
    rd_kafka_conf_set(conf, "batch.num.messages", "1000000", NULL, 0);
    rd_kafka_conf_set(conf, "queue.buffering.max.ms", "1000", NULL, 0);
    rd_kafka_conf_set(conf, "queue.buffering.max.kbytes", "5120000", NULL, 0);
    rd_kafka_conf_set(conf, "queue.buffering.max.messages", "10000000", NULL, 0);
//    rd_kafka_conf_set(conf, "debug", "fetch,metadata,topic,broker", NULL, 0);

    // 设置Kafka代理地址，根据你的实际情况修改
    const char *brokers = "192.168.1.131:19092,192.168.1.132:19092,192.168.1.133:19092";
    conf_res = rd_kafka_conf_set(conf, "bootstrap.servers", brokers, NULL, 0);
    if (conf_res != RD_KAFKA_CONF_OK) {
        fprintf(stderr, "Error setting bootstrap.servers: %s\n", rd_kafka_err2str(conf_res));
        pthread_exit(NULL);
    }

    // 创建Kafka生产者
    rd_kafka_t *rk;
    rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, NULL, 0);
    if (!rk) {
        fprintf(stderr, "Error creating producer\n");
        pthread_exit(NULL);
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
        rd_kafka_destroy(rk);
        pthread_exit(NULL);
    }

    // 生产消息
    const char *message_payload = "{ \"source_ip\": \"8.8.8.8\", \
                                     \"source_port\": \"123\", \
                                     \"destination_ip\": \"123.123.123.213\", \
                                     \"destination_port\": \"456\", \
                                     \"protocol\": \"TCP\", \
                                     \"timestamp\": 5555, \
                                     \"id\": 6666 }";
    size_t len = strlen(message_payload);

    // 发送消息
    produce_messages(rk, rkt, message_payload, len, num_messages_sum / NUM_THREADS);

    // 销毁Kafka生产者和相关资源
    rd_kafka_topic_destroy(rkt);
    rd_kafka_destroy(rk);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    num_messages_sum = atoi(argv[1]);
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // 启动多个线程
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, thread_function, (void *)&thread_ids[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // 获取当前时间
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    // 等待所有线程结束
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // 获取结束时间
    gettimeofday(&end_time, NULL);

    // 计算用时
    long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000 +
                        (end_time.tv_usec - start_time.tv_usec);

    // 打印结果
    fprintf(stdout, "Messages sent: %d\n", num_messages_sum);
    fprintf(stdout, "Elapsed time: %ld microseconds\n", elapsed_time);

    return 0;
}
