#include "rom.h"
#include "EEPROM.h"
void ROM_init(){
  EEPROM.begin(512);
}

// 安全写入数据到EEPROM
bool ROM_write(const char* data, const AddressList* list) {
  if (!data || !list) return false;
  
  unsigned int index = list->address;
  unsigned int maxAddr = list->address + list->stringLen;
  unsigned int dataLen = strlen(data);
  
  // 边界检查
  if (maxAddr > EEPROM.length()) {
    Serial.println("Error: EEPROM address overflow");
    return false;
  }

  // 写入数据
  for (unsigned int i = 0; i < list->stringLen; ++i) {
    if (i < dataLen) {
      EEPROM.write(index++, data[i]);
    } else {
      EEPROM.write(index++, '\0'); // 剩余空间填终止符
    }
  }
  // delay(10);
  return EEPROM.commit();
}

// 读取单个字节
unsigned char ROM_readByte(unsigned char addr) {
  if (addr >= EEPROM.length()) {
    Serial.println("Error: Address out of range");
    return 0xFF;
  }
  return EEPROM.read(addr);
}

// 读取字符串数据
String ROM_readString(const AddressList* list) {
  if (!list) return "";

  String tempData;
  tempData.reserve(list->stringLen); // 预分配内存
  
  unsigned int endAddr = list->address + list->stringLen;
  
  for (unsigned int i = list->address; i < endAddr; ++i) {
    char c = ROM_readByte(i);
    if (c == '\0') break; // 遇到终止符停止
    tempData += c;
  }
  
  return tempData;
}