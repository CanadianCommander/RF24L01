#include <Arduino.h>
#include "RF24L01.h"

//SPI ----------------------------
void initSPI();
uint8_t transmitSPI(uint8_t msg);
//////////////////////////////////
// RF24L01 ----------------------
#define CE_PIN PORTB1
#define CSN_PIN PORTB2
void initRF24L01();
uint8_t writeRegRF24L01(uint8_t addr, uint8_t reg);
uint8_t writeRegRF24L01(uint8_t addr, uint8_t * data, uint8_t len);
uint8_t readRegRF24L01(uint8_t addr);
uint8_t writeTxPayload(uint8_t data);
uint8_t readRxPayload(uint8_t * buffer, uint8_t len);
uint8_t writeTxPayload(uint8_t * data, uint8_t len);
uint8_t getStatus();
uint8_t flushRx();

void powerUpRF24L01();
void setTxAddress(uint8_t * addr, uint8_t len);
void setRxAddress(uint8_t * addr, uint8_t len);
void transmitRF24L01(uint8_t data);
void receiveRF24L01(uint8_t * buffer, uint8_t len);
uint8_t HasRxData();
void listenForTransmission();
void printStatus(uint8_t sReg);
/////////////////////////////////


uint8_t myAddr[5] = {0xB3,0xB4,0xB5,0xB6,0x05};
void setup(){
  Serial.begin(9600);
  Serial.print("INIT....");
  initSPI();
  initRF24L01();
  setRxAddress(myAddr, 5);
  setTxAddress(myAddr,5);
  listenForTransmission();
  Serial.print("INIT Done\n");
}

uint8_t dataBuff[33];
void loop(){
  //delay(1000);
  //transmitRF24L01(0);


  delay(1000);
  if(HasRxData()){
    Serial.print("I have mail: ");
    receiveRF24L01(dataBuff,33);
    for(int i =0; i < 33; i ++){
      Serial.print(dataBuff[i], BIN);
      Serial.print(" ");
    }
    Serial.print("\n");
  }
  Serial.print(readRegRF24L01(RPD), BIN);
}


void initSPI(){
  //clock and master out to output
  DDRB = (1 << PORTB3) | (1 << PORTB5) | (1 << PORTB2);
  DDRB &= ~(1 << PORTB4);
  // enable spi, set master, prescalar 64.
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
}

uint8_t transmitSPI(uint8_t msg){
  SPDR = msg;
  //wait for transmit to complete
  while(!(SPSR & 0x80));
  return SPDR;
}

void initRF24L01(){
  //PORTB1/2 to output. B1 is CE and B2 is CSN.
  DDRB |= (1 << CE_PIN) | (1 << CSN_PIN);
  PORTB |= (1 << CSN_PIN);
  PORTB &= ~(1 << CE_PIN);

  powerUpRF24L01();
  delay(2);
  writeRegRF24L01(FEATURE,(1 << EN_DPL) | (1 << EN_ACK_PAY) | (1 << EN_DYN_ACK));
  writeRegRF24L01(DYNPD,(1 << DPL_P5) | (1 << DPL_P4) | (1 << DPL_P3) | (1 << DPL_P2) | (1 << DPL_P1) | (1 << DPL_P0));
  writeRegRF24L01(SETUP_RETR, (0xF << ARDa) | (0xF << ARC));
  writeRegRF24L01(EN_RXADDR, (1 << ERX_P5) | (1 << ERX_P4) | (1 << ERX_P3) | (1 << ERX_P2) | (1 << ERX_P1) | (1 << ERX_P0));
}

uint8_t writeRegRF24L01(uint8_t addr, uint8_t reg){
  return writeRegRF24L01(addr,&reg,1);
}

uint8_t writeRegRF24L01(uint8_t addr, uint8_t * data, uint8_t len){
  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(W_REGISTER(addr));
  for(int i =0; i < len; i ++ ){
    transmitSPI(*(data + i));
  }
  PORTB |= (1 << CSN_PIN);
  return status;
}

uint8_t readRegRF24L01(uint8_t addr){
  PORTB &= ~(1 << CSN_PIN);
  transmitSPI(R_REGISTER(addr));
  uint8_t res = transmitSPI(0x0);
  PORTB |= (1 << CSN_PIN);
  return res;
}

uint8_t readRxPayload(uint8_t * buffer, uint8_t len){
  //zero buffer
  memset(buffer,0,len);

  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(R_RX_PAYLOAD);

  for(int i =0; i < len; i ++){
    buffer[i] = transmitSPI(0x1);
  }
  PORTB |= (1 << CSN_PIN);
  return status;
}

uint8_t writeTxPayload(uint8_t data){
  return writeTxPayload(&data,1);
}

uint8_t writeTxPayload(uint8_t * data, uint8_t len){
  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(W_TX_PAYLOAD);
  for(int i =0; i < len; i ++){
    transmitSPI(*(data + i));
  }

  PORTB |= (1 << CSN_PIN);
  return status;
}

uint8_t getStatus(){
  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(NOP);
  PORTB |= (1 << CSN_PIN);
  return status;
}


uint8_t flushRx(){
  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(FLUSH_RX);
  PORTB |= (1 << CSN_PIN);
  return status;
}


void transmitRF24L01(uint8_t data){
  //configure for transmit.
  printStatus(writeRegRF24L01(CONFIG, 0xA));
  printStatus(writeTxPayload(0xBB));

  //transmit packet(s).
  PORTB |= (1 << CE_PIN);
  delay(1000);
  PORTB &= ~(1 << CE_PIN);
}

void receiveRF24L01(uint8_t * buffer, uint8_t len){
  readRxPayload(buffer,len);
}

void powerUpRF24L01(){
  printStatus(writeRegRF24L01(CONFIG,0x02));
}

void setTxAddress(uint8_t * addr, uint8_t len){
  writeRegRF24L01(TX_ADDR,addr,len);// transmit target
  writeRegRF24L01(RX_ADDR_P0,addr,len);//rcv ack on pipe 0.
}

void setRxAddress(uint8_t * addr, uint8_t len){
  writeRegRF24L01(RX_ADDR_P5,addr,len);
}

uint8_t HasRxData(){
  uint8_t status = getStatus();
  printStatus(status);
  return ((status >> RX_P_NO) & 0x7) != 0x7;
}

void listenForTransmission(){
  writeRegRF24L01(CONFIG, 0xB);
  PORTB |= (1 << CE_PIN);
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
