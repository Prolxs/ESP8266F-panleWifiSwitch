#include "pc_module.h"

static void pcPWR_High(){
  digitalWrite(PC_PWR, 1);
}

static void pcPWR_Low(){
  digitalWrite(PC_PWR, 0);
}

static void pcRST_High(){
  digitalWrite(PC_RST, 1);
}

static void pcRST_Low(){
  digitalWrite(PC_RST, 0);
}

void pcModule_init(){
  pinMode(PC_PWR, OUTPUT);
  pinMode(PC_RST, OUTPUT);
  pinMode(PC_HDDLED_INPUT, INPUT);
  pinMode(PC_PWRLED_INPUT, INPUT);
  pcPWR_Low();
  pcRST_Low();
}

void PC_startUp(){
  pcPWR_High();
  delay(1000);
  pcPWR_Low();
}

void PC_shutDown(){
  pcPWR_High();
  delay(5000);
  pcPWR_Low();
}

void PC_reStart(){
  pcRST_High();
  delay(5000);
  pcRST_Low(); 
}

int readPowerLed(){
  return digitalRead(PC_PWRLED_INPUT);
}