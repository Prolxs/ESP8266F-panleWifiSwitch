#include "pc_module.h"
#include "check.h"
#include "rom.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include "fonts.h"
 
#define SCREEN_WIDTH 128 // 设置OLED宽度,单位:像素
#define SCREEN_HEIGHT 32 // 设置OLED高度,单位:像素
#define OLED_RESET 15
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 定义 AP 参数和 UDP 端口
const char* ap_ssid = "Phuck_PCB";
const char* ap_password = "";
const unsigned int udpPort = 8888;  // UDP 监听端口
WiFiUDP udp;
WiFiCredentials wifiData;
AddressList dataList[7]={
  {0x00, 16},//gateip
  {0x16, 16},//localip
  {0x32, 16},//ssid
  {0x48, 16},//pwd
  {0x64, 2}, //绑定状态
  {0x66, 16}, //remote ip
  {0x82, 4} //remote port
};

void setup(){
  pcModule_init();
  OLED_init();
  Serial.begin(115200);
  ROM_init();
  readRom_init();
  WiFi_init(); 
  // Serial.printf("gateIP:%s, len:%d\n",ROM_readString(&dataList[0]).c_str(), ROM_readString(&dataList[0]).length());
  // Serial.printf("localIP:%s, len:%d\n",ROM_readString(&dataList[1]).c_str(), ROM_readString(&dataList[1]).length());
  // Serial.printf("ssid:%s, len:%d\n",wifiData.ssid, ROM_readString(&dataList[2]).length());
  // Serial.printf("pwd:%s, len:%d\n",wifiData.password, ROM_readString(&dataList[3]).length());
  // Serial.printf("status:%s, len:%d\n",status ,ROM_readString(&dataList[4]).length());
}
 
void loop(){
  // if(WiFi.status() != WL_CONNECTED)wifiData.WIFI_CONNECT_STATUS = false;
  if(wifiData.WIFI_BINDSTATUS == true && wifiData.WIFI_CONNECT_STATUS == false){
    if(Connect(wifiData.ssid, wifiData.password) == true){
      wifiData.WIFI_CONNECT_STATUS = true;
      display.setCursor(0,15);
      display.printf("IP:");
      display.println(WiFi.localIP());
    }
  }
  watcher();
  Handle();
  wifiLoop();
  display.display(); //刷新显存 
}

void OLED_init(){
  Wire.begin(5,4);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.clearDisplay();
}

void readRom_init(){
  char status[3];
  strncpy(wifiData.gate_ip, ROM_readString(&dataList[0]).c_str(), 16);
  strncpy(wifiData.local_ip, ROM_readString(&dataList[1]).c_str(), 16);
  strncpy(wifiData.ssid, ROM_readString(&dataList[2]).c_str(), MAX_SSID_LEN - 1);
  strncpy(wifiData.password, ROM_readString(&dataList[3]).c_str(), MAX_PWD_LEN - 1);
  strncpy(status, ROM_readString(&dataList[4]).c_str(), dataList[4].stringLen);
  if(status[0]=='1')wifiData.WIFI_BINDSTATUS = true;
  else wifiData.WIFI_BINDSTATUS = false;
}

void WiFi_init(){
  if(wifiData.WIFI_BINDSTATUS==true) WiFi.mode(WIFI_STA);
  else WiFi.mode(WIFI_AP_STA);
  // WiFi.mode(WIFI_AP_STA);
  // 定义自定义 IP 参数
  IPAddress local_IP(192, 168, 1, 1);      // AP 的 IP 地址
  IPAddress gateway(192, 168, 1, 1);       // 网关（通常与 IP 相同）
  IPAddress subnet(255, 255, 255, 0);      // 子网掩码
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ap_ssid, ap_password);
  udp.begin(udpPort);

  display.clearDisplay();
  display.setCursor(0, 4);
  display.print("gateIP:");
  display.println(WiFi.softAPIP());
  Serial.println(WiFi.softAPIP());
}

void wifiLoop(){
  // 检查是否有 UDP 数据包到达
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.printf("Received %d bytes from %s:%d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    // 读取数据内容
    char packetBuffer[255];
    int len = udp.read(packetBuffer, sizeof(packetBuffer));
    if (len > 0) {
      packetBuffer[len] = '\0';  // 确保字符串终止
      Serial.print("Data: ");
      Serial.println(packetBuffer);
      strncpy(wifiData.remote_ip, udp.remoteIP().toString().c_str(), 17);
      wifiData.remote_port = udp.remotePort();
      if(wifiData.WIFI_BINDSTATUS==true && wifiData.WIFI_CONNECT_STATUS==true) megReceive(packetBuffer, &wifiData);
      if(wifiData.WIFI_BINDSTATUS==false) Bind(packetBuffer, &wifiData);

      Serial.println(udp.remoteIP());
    }
  }
}

/**
  在没有sta账户密码的情况下尝试连接给的wifi密码，连接成功后将绑定标志位置位并写入
  并且关闭AP模式的wifi
**/
void Bind(char* packetBuffer, WiFiCredentials* credentials){
  if(extractCredentials(packetBuffer, &wifiData)==true){ //wifi账户数据检查
    Connect(wifiData.ssid,wifiData.password);
    if (WiFi.status() == WL_CONNECTED) {
      wifiData.WIFI_BINDSTATUS = true; //已绑定
      WiFi.mode(WIFI_STA);
      ROM_write(WiFi.localIP().toString().c_str(), &dataList[1]);//存储IP
      ROM_write(wifiData.ssid, &dataList[2]); //ssid
      ROM_write(wifiData.password, &dataList[3]); //pwd
      ROM_write("10", &dataList[4]);//1：已绑定 ,AB A:绑定 B未定

      megSend(("IP="+WiFi.localIP().toString()+"PORT=8888").c_str());
      display.setCursor(0, 16);
      display.print("localIP:");
      display.println(WiFi.localIP());

      Serial.println("\nSTA Connected!");
      Serial.print("STA IP: ");
      Serial.println(WiFi.localIP()); // 显示从路由器获取的 IP
    } else {
      Serial.println("\nSTA Connection Failed!");
    }
  }
}

bool Connect(char* ssid,char* pwd){
  WiFi.begin(ssid, pwd);//连接
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 5000) {
    delay(100);
    Serial.print(".");
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.print("\n连接成功!");
    return true;
  }
  else return false;
}

void megReceive(char* packetBuffer, WiFiCredentials* credentials){
  parseData(packetBuffer, &wifiData);
}

void megSend(const char* meg) {
    udp.beginPacket(wifiData.remote_ip, wifiData.remote_port); // 固定端口
    udp.write(meg);
    udp.endPacket();
}

void Handle(){
  if(wifiData.PWR == 3){
    if(wifiData.PC_POWER==true){
      megSend("POWER:ON");
      Serial.println("ON\n");
    }
    else{
      megSend("POWER:OFF");
      Serial.println("OFF\n");
    }
    wifiData.PWR = 0;
  }
  if(wifiData.PWR == 1){ //开机
  PC_startUp();
  wifiData.PWR = 0;
  }
  if(wifiData.PWR == 2){ //关机
    PC_shutDown();
    wifiData.PWR = 0;
  }
  if(wifiData.PRST == 1){
    PC_reStart();
    wifiData.PRST = 0;  
  }
  if(wifiData.RST == 1){
    wifiData.RST = 0;
    WiFi.mode(WIFI_AP_STA);
    wifiData.WIFI_BINDSTATUS = false;
    display.clearDisplay();
    display.setCursor(0, 16);
    display.println("No link");
  }
}

void watcher(){
  static uint8_t status = 0;
  uint8_t temp = readPowerLed();
  if(status != temp){
    if(temp == 0){
      wifiData.PC_POWER = false;
      megSend("POWER:OFF");
    }
    else if(temp == 1){
      wifiData.PC_POWER = true;
      megSend("POWER:ON");
    }
    status = temp;
    // Serial.print(wifiData.PC_POWER);
  }
}