#ifndef _PC_MODULE_H
#define _PC_MODULE_H
#define PC_PWRLED_INPUT 16
#define PC_HDDLED_INPUT 14
#define PC_PWR 12
#define PC_RST 13
void PC_startUp();
void PC_shutDown();
void PC_reStart();
int readPowerLed();
void pcModule_init();
#endif