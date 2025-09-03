#ifndef _CHECK_H
#define _CHECK_H
// 定义最大长度常量
#define MAX_SSID_LEN 16
#define MAX_PWD_LEN 16

// 使用结构体存储凭证，避免全局变量
typedef struct {
    char ssid[MAX_SSID_LEN];
    char password[MAX_PWD_LEN];
    char gate_ip[17];
    char local_ip[17];
    char remote_ip[17];
    uint16_t remote_port;
    bool WIFI_BINDSTATUS = false;
    bool WIFI_CONNECT_STATUS = false;
    bool PC_POWER = false;
    unsigned char PWR = 0; //电脑开关
    unsigned char PRST = 0; //电脑重启
    unsigned char RST = 0; //电路板重置
} WiFiCredentials;
bool extractCredentials(const char* raw, WiFiCredentials* credentials);
bool parseData(const String &input, WiFiCredentials* credentials);
#endif