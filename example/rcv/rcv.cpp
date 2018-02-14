#include <Arduino.h>

#define CE_PIN PORTB1
#define CSN_PIN PORTB2
#include "RF24L01.h"
using namespace RF24L01;

void printStatus(uint8_t sReg);

uint8_t myAddr[5] = {0xB3,0xB4,0xB5,0xB6,0x06};
void setup(){
  Serial.begin(9600);
  Serial.print("INIT....");

  initRF24L01();
  setReceiveAddress(RX_ADDR_P0,myAddr, 5);
  configureAsReceiver();

  setChannel(2);
  setDataRate(2);//250 kbs
  setRetransmitTime(2);// 500 nano

  Serial.print("INIT Done\n");
}

uint8_t dataBuff[33];
void loop(){
  delay(1000);
  if(HasReceiveData()){
    setResponseMsg(0,(uint8_t*)"YO!",4);
    Serial.print("I have mail: ");
    getReceivedMsg(dataBuff,33);
    Serial.print((char*)dataBuff);
    Serial.print("\n");
  }
  Serial.print(readRegRF24L01(RPD), BIN);
  printStatus(getStatus());
}


void printStatus(uint8_t sReg){
  Serial.print(" Have RX data: ");
  Serial.print((sReg >> RX_DR));
  Serial.print(" TX data Sent: ");
  Serial.print((sReg & 0x20) >> TX_DS);
  Serial.print(" RX data backlog: ");
  Serial.print((sReg & 0xE) >> 1);
  Serial.print(" TX Full: ");
  Serial.print((sReg & 0x1));
  Serial.print("\n");
}
