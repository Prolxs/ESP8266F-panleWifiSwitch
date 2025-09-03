#ifndef _ROM_H_
#define _ROM_H_
#define IPLEN 16

typedef struct{
  unsigned int address = 0;
  unsigned char stringLen = 0;
}AddressList;
void ROM_init();
bool ROM_write(const char* data, const AddressList* list);
String ROM_readString(const AddressList* list);
#endif