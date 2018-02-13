#define CE_PIN PORTB1
#define CSN_PIN PORTB2
#include "RF24L01.h"

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
  writeRegRF24L01(RF_CH,0x01);
}

void configRX(){
  writeRegRF24L01(EN_RXADDR, (1 << ERX_P5) | (1 << ERX_P4) | (1 << ERX_P3) | (1 << ERX_P2) | (1 << ERX_P1) | (1 << ERX_P0));
  writeRegRF24L01(EN_AA, 0x1F);
  listenForTransmission();
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
    buffer[i] = transmitSPI(0x0);
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
  transmitRF24L01(&data,1);
}

void transmitRF24L01(uint8_t * data, uint8_t len){
  //configure for transmit.
  writeRegRF24L01(CONFIG, 0xA);
  writeTxPayload(data,len);

  //transmit packet(s).
  PORTB |= (1 << CE_PIN);
  //wait for transmit finish
  while(((getStatus() >> TX_DS) & 0x1) != 0x1){
    if(((getStatus() >> MAX_RT) & 0x1) == 0x1){
      break;
    }
  }
  PORTB &= ~(1 << CE_PIN);

  //clear transmit complete & timeout status bit
  writeRegRF24L01(STATUS, readRegRF24L01(STATUS) | ( 1 << TX_DS) | (1 << MAX_RT));
}

void receiveRF24L01(uint8_t * buffer, uint8_t len){
  readRxPayload(buffer,len);
  //clear rcv status bit.
  writeRegRF24L01(STATUS, readRegRF24L01(STATUS) | ( 1 << RX_DR));
}

void powerUpRF24L01(){
  writeRegRF24L01(CONFIG,0x02);
}

void setTxAddress(uint8_t * addr, uint8_t len){
  writeRegRF24L01(TX_ADDR,addr,len);// transmit target
  setRxAddress(addr,len);//rcv ack on pipe 0.
}

void setRxAddress(uint8_t * addr, uint8_t len){
  writeRegRF24L01(RX_ADDR_P0,addr,len);
  writeRegRF24L01(RX_ADDR_P1,addr[len-1] + 1);
  writeRegRF24L01(RX_ADDR_P2,addr[len-1] + 2);
  writeRegRF24L01(RX_ADDR_P3,addr[len-1] + 3);
  writeRegRF24L01(RX_ADDR_P4,addr[len-1] + 4);
  writeRegRF24L01(RX_ADDR_P5,addr[len-1] + 5);
}

uint8_t HasRxData(){
  uint8_t status = getStatus();
  return ((status >> RX_P_NO) & 0x7) != 0x7;
}

void listenForTransmission(){
  writeRegRF24L01(CONFIG, 0xB);
  PORTB |= (1 << CE_PIN);
}
