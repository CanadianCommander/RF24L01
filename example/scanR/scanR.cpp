#include <Arduino.h>

#define CE_PIN PORTB1
#define CSN_PIN PORTB2
#include "RF24L01.h"
using namespace RF24L01;

void printStatus(uint8_t sReg);

uint8_t chan = 4;
uint8_t myAddr[5] = {0xB3,0xB4,0xB5,0xB6,0x06};
void setup(){
  Serial.begin(9600);
  Serial.print("INIT....");

  initRF24L01();
  setTransmitAddress(myAddr,5);

  setChannel(2);

  Serial.print("INIT Done\n");

  for(;;){
    delay(250);
    bool res = transmitMsg((uint8_t*)"HELLO?",7);
    if(!res){
      //must go up by 2 channels in 2Mbps mode.
      chan+=2;
      if(chan > 128){
        chan=0;
      }
      setChannel(chan);
      Serial.print("SCANNING CHANNEL: ");
      Serial.print(chan);
      Serial.print("\n");
    }
    else{
      Serial.print("FOUND A FREIND!  :3\n");
      break;
    }
  }
}

uint8_t dataBuff[33];
void loop(){

  delay(1000);
  transmitMsg((uint8_t*)"HELLO WORLD", 12);
  if(hasReceiveData()){
    getReceivedMsg(dataBuff,32);
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
