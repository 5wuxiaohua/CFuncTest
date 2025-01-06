#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_CIDR_PREFIX_LENGTH 32

// Trie 树节点
typedef struct TrieNode {
    struct TrieNode *children[2]; // 子节点，0或1
    char **cidrs;                 // 匹配的CIDR地址段
    int cidr_count;               // 匹配的CIDR地址段数目
} TrieNode;

// 创建一个新的 Trie 节点
TrieNode* createNode() {
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    node->children[0] = NULL;
    node->children[1] = NULL;
    node->cidrs = NULL;
    node->cidr_count = 0;
    return node;
}

// 将IPv4地址（如 "192.168.1.1"）转换为32位整数
uint32_t ipToLong(const char *ip) {
    uint32_t result = 0;
    int octet;
    const char *ptr = ip;
    
    for (int i = 3; i >= 0; --i) {
        sscanf(ptr, "%d", &octet);
        result |= (octet << (i * 8));
        while (*ptr && *ptr != '.') ptr++;
        if (*ptr) ptr++;
    }
    return result;
}

// 将CIDR地址段插入前缀树
void insertCIDR(TrieNode *root, const char *cidr) {
    char ip[16];
    int prefix_length;
    
    // 解析CIDR地址段
    sscanf(cidr, "%15[^/]/%d", ip, &prefix_length);
    
    uint32_t ip_binary = ipToLong(ip);
    
    TrieNode *node = root;
    for (int i = 0; i < prefix_length; ++i) {
        int bit = (ip_binary >> (31 - i)) & 1;
        if (node->children[bit] == NULL) {
            node->children[bit] = createNode();
        }
        node = node->children[bit];
    }

    // 保存CIDR到当前节点
    if (node->cidrs == NULL) {
        node->cidrs = (char **)malloc(sizeof(char *));
    } else {
        node->cidrs = (char **)realloc(node->cidrs, (node->cidr_count + 1) * sizeof(char *));
    }
    node->cidrs[node->cidr_count] = strdup(cidr);
    node->cidr_count++;
}

// 查询IP地址匹配的CIDR地址段
void matchCIDR(TrieNode *root, const char *ip) {
    uint32_t ip_binary = ipToLong(ip);
    
    TrieNode *node = root;
    int match_found = 0;

    for (int i = 0; i < MAX_CIDR_PREFIX_LENGTH; ++i) {
        int bit = (ip_binary >> (31 - i)) & 1;
        if (node->children[bit] != NULL) {
            node = node->children[bit];
            // 如果当前节点有匹配的CIDR地址段，打印它们
            if (node->cidrs != NULL && node->cidr_count > 0) {
                for (int j = 0; j < node->cidr_count; ++j) {
                    printf("%s\n", node->cidrs[j]);
                }
                match_found = 1;
            }
        } else {
            break;
        }
    }

    if (!match_found) {
        printf("No matching CIDR found for IP %s\n", ip);
    }
}

// 释放Trie树内存
void freeTrie(TrieNode *node) {
    if (node == NULL) return;
    
    for (int i = 0; i < 2; ++i) {
        freeTrie(node->children[i]);
    }
    
    if (node->cidrs != NULL) {
        for (int i = 0; i < node->cidr_count; ++i) {
            free(node->cidrs[i]);
        }
        free(node->cidrs);
    }
    
    free(node);
}

// 主函数
int main() {
    TrieNode *root = createNode();

    // 假设这是我们要插入的CIDR地址段
    char *cidr_list[] = {
        "192.168.1.0/24",
        "192.168.0.0/22",
        "10.0.0.0/8",
        "172.16.0.0/12"
    };
    
    int cidr_count = sizeof(cidr_list) / sizeof(cidr_list[0]);

    // 将CIDR地址段插入Trie树
    for (int i = 0; i < cidr_count; ++i) {
        insertCIDR(root, cidr_list[i]);
    }

    // 测试查询IP地址
    char test_ip[] = "192.168.1.5";
    printf("Matching CIDRs for IP %s:\n", test_ip);
    matchCIDR(root, test_ip);
    
    // 释放Trie树内存
    freeTrie(root);
    
    return 0;
}
