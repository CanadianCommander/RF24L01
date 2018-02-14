#include <Arduino.h>

#define CE_PIN PORTB1
#define CSN_PIN PORTB2
#include "RF24L01.h"

void printStatus(uint8_t sReg);

uint8_t myAddr[5] = {0xB3,0xB4,0xB5,0xB6,0x05};
void setup(){
  Serial.begin(9600);
  Serial.print("INIT....");
  initSPI();
  initRF24L01();
  setTxAddress(myAddr,5);
  Serial.print("INIT Done\n");
}

uint8_t dataBuff[33];
void loop(){
  delay(1000);
  transmitRF24L01((uint8_t*)"HELLO WORLD", 12);
  if(HasReceiveData()){
    receiveRF24L01(dataBuff,32);
    Serial.print((char*)dataBuff);
    Serial.print("\n");
  }
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
