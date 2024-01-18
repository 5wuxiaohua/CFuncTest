#include <stdio.h>

int main() {
    char result[100];  // 字符数组用于存储拼接后的字符串
    int chars_written = 0;  // 记录已写入的字符数

    const char *str1 = "Hello";
    const char *str2 = "World";
    int int1 = 42;
    int int2 = 99;

    // 第一次拼接，将字符串 str1 追加到 result
    chars_written += snprintf(result + chars_written, sizeof(result) - chars_written, "%s", str1);

    // 第二次拼接，将字符串 str2 追加到 result
    chars_written += snprintf(result + strlen(result), sizeof(result) - strlen(), ", %s", str2);

    // 第三次拼接，将整型 int1 追加到 result
    chars_written += snprintf(result + chars_written, sizeof(result) - chars_written, ", %d", int1);

    // 第四次拼接，将整型 int2 追加到 result
    chars_written += snprintf(result + chars_written, sizeof(result) - chars_written, ", %d", int2);


    // // 第一次拼接，将字符串 str1 追加到 result
    // chars_written += sprintf(result + chars_written, "%s", str1);

    // // 第二次拼接，将字符串 str2 追加到 result
    // chars_written += sprintf(result + chars_written, ", %s", str2);

    // // 第三次拼接，将整型 int1 追加到 result
    // chars_written += sprintf(result + chars_written, ", %d", int1);

    // // 第四次拼接，将整型 int2 追加到 result
    // chars_written += sprintf(result + chars_written, ", %d", int2);

    // 打印拼接后的字符串
    printf("Concatenated String: %s\n", result);

    return 0;
}
