#define CE_PIN PORTB1
#define CSN_PIN PORTB2
#include "RF24L01.h"
using namespace RF24L01;

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

void RF24L01::initRF24L01(){
  initSPI();
  //PORTB CE and CSN to output.
  DDRB |= (1 << CE_PIN) | (1 << CSN_PIN);
  PORTB |= (1 << CSN_PIN);
  PORTB &= ~(1 << CE_PIN);

  powerUp();
  //enable all features
  writeRegRF24L01(FEATURE,(1 << EN_DPL) | (1 << EN_ACK_PAY) | (1 << EN_DYN_ACK));
  writeRegRF24L01(DYNPD,(1 << DPL_P5) | (1 << DPL_P4) | (1 << DPL_P3) | (1 << DPL_P2) | (1 << DPL_P1) | (1 << DPL_P0));
  //max retransmit delay and max retransmit count.
  writeRegRF24L01(SETUP_RETR, (0xF << ARDa) | (0xF << ARC));
  setChannel(1);
}

void RF24L01::configureAsReceiver(){
  writeRegRF24L01(EN_RXADDR, (1 << ERX_P5) | (1 << ERX_P4) | (1 << ERX_P3) | (1 << ERX_P2) | (1 << ERX_P1) | (1 << ERX_P0));
  writeRegRF24L01(EN_AA, 0x3F);
  listenForTransmission();
}

uint8_t RF24L01::writeRegRF24L01(uint8_t addr, uint8_t reg){
  return writeRegRF24L01(addr,&reg,1);
}

uint8_t RF24L01::writeRegRF24L01(uint8_t addr, uint8_t * data, uint8_t len){
  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(W_REGISTER(addr));
  for(int i =0; i < len; i ++ ){
    transmitSPI(*(data + i));
  }
  PORTB |= (1 << CSN_PIN);
  return status;
}

uint8_t RF24L01::readRegRF24L01(uint8_t addr){
  PORTB &= ~(1 << CSN_PIN);
  transmitSPI(R_REGISTER(addr));
  uint8_t res = transmitSPI(0x0);
  PORTB |= (1 << CSN_PIN);
  return res;
}

uint8_t RF24L01::readRxPayload(uint8_t * buffer, uint8_t len){
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

uint8_t RF24L01::writeTxPayload(uint8_t data){
  return writeTxPayload(&data,1);
}

uint8_t RF24L01::writeTxPayload(uint8_t * data, uint8_t len){
  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(W_TX_PAYLOAD);
  for(int i =0; i < len; i ++){
    transmitSPI(*(data + i));
  }

  PORTB |= (1 << CSN_PIN);
  return status;
}

uint8_t RF24L01::writeRxPayload(uint8_t pipe, uint8_t data){
  return writeRxPayload(pipe, &data,1);
}

uint8_t RF24L01::writeRxPayload(uint8_t pipe, uint8_t * data, uint8_t len){
  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(W_ACK_PAYLOAD(pipe));
  for(uint8_t i = 0; i< len; i ++){
    transmitSPI(*(data + i));
  }
  PORTB |= (1 << CSN_PIN);
  return status;
}

uint8_t RF24L01::getStatus(){
  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(NOP);
  PORTB |= (1 << CSN_PIN);
  return status;
}


uint8_t RF24L01::flushReceiveBuffer(){
  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(FLUSH_RX);
  PORTB |= (1 << CSN_PIN);
  return status;
}

uint8_t RF24L01::flushTransmitBuffer(){
  PORTB &= ~(1 << CSN_PIN);
  uint8_t status = transmitSPI(FLUSH_TX);
  PORTB |= (1 << CSN_PIN);
  return status;
}


bool RF24L01::transmitMsg(uint8_t data){
  return transmitMsg(&data,1);
}

bool RF24L01::transmitMsg(uint8_t * data, uint8_t len){
  bool transmitSuccess = true;
  //configure for transmit.
  writeRegRF24L01(CONFIG, 0xA);
  writeTxPayload(data,len);

  //transmit packet(s).
  PORTB |= (1 << CE_PIN);
  //wait for transmit finish
  while(((getStatus() >> TX_DS) & 0x1) != 0x1){
    if(((getStatus() >> MAX_RT) & 0x1) == 0x1){
      flushTransmitBuffer();
      transmitSuccess = false;
      break;
    }
  }
  PORTB &= ~(1 << CE_PIN);

  //clear transmit complete & timeout status bit
  writeRegRF24L01(STATUS, readRegRF24L01(STATUS) | ( 1 << TX_DS) | (1 << MAX_RT));

  return transmitSuccess;
}

void RF24L01::getReceivedMsg(uint8_t * buffer, uint8_t len){
  readRxPayload(buffer,len);
  //clear rcv status bit.
  writeRegRF24L01(STATUS, readRegRF24L01(STATUS) | ( 1 << RX_DR));
}

uint8_t RF24L01::getReceivePipe(){
  return ((getStatus() >> RX_P_NO) & 0xF8);
}

void RF24L01::setResponseMsg(uint8_t pipe, uint8_t data){
  setResponseMsg(pipe,&data,1);
}

void RF24L01::setResponseMsg(uint8_t pipe, uint8_t * buffer, uint8_t len){
  writeRxPayload(pipe,buffer,len);
}

void RF24L01::powerUp(){
  writeRegRF24L01(CONFIG,0x02);
}

void RF24L01::powerDown(){
  writeRegRF24L01(CONFIG,0x00);
}

bool RF24L01::setTransmitAddress(uint8_t * addr, uint8_t len){
  writeRegRF24L01(TX_ADDR,addr,len);// transmit target
  writeRegRF24L01(RX_ADDR_P0,addr,len);//rcv ack on pipe 0.
  return true;
}

bool RF24L01::setReceiveAddress(uint8_t * addr, uint8_t len){
  writeRegRF24L01(RX_ADDR_P0,addr,len);
  writeRegRF24L01(RX_ADDR_P1,addr[len-1] + 1);
  writeRegRF24L01(RX_ADDR_P2,addr[len-1] + 2);
  writeRegRF24L01(RX_ADDR_P3,addr[len-1] + 3);
  writeRegRF24L01(RX_ADDR_P4,addr[len-1] + 4);
  writeRegRF24L01(RX_ADDR_P5,addr[len-1] + 5);
  return true;
}

bool RF24L01::setReceiveAddress(uint8_t pipe, uint8_t * addr, uint8_t len){
  if(pipe == RX_ADDR_P0){
    writeRegRF24L01(pipe,addr,len);
    return true;
  }
  else if(pipe >= RX_ADDR_P1 && pipe <= RX_ADDR_P5){
    if(len == 1){
      writeRegRF24L01(pipe,addr,len);
      return true;
    }
  }
  return false;
}

bool RF24L01::setChannel(uint8_t channel){
  if(channel < 0x80){
    writeRegRF24L01(RF_CH,channel);
    return true;
  }
  else{
    return false;
  }
}

bool RF24L01::setRetransmitTime(uint8_t rTime){
  if(rTime < 0x10){
    writeRegRF24L01(SETUP_RETR, (readRegRF24L01(SETUP_RETR) & 0x0F) | (rTime << ARDa));
    return true;
  }
  else{
    return false;
  }
}

bool RF24L01::setRetransmitCount(uint8_t rCount){
  if(rCount < 0x10){
    writeRegRF24L01(SETUP_RETR, (readRegRF24L01(SETUP_RETR) & 0xF0) | rCount);
    return true;
  }
  else{
    return false;
  }
}

bool RF24L01::setDataRate(uint8_t dRate){
  switch (dRate){
    case 0:
      writeRegRF24L01(RF_SETUP, (readRegRF24L01(RF_SETUP) & 0xD7));
      return true;
    case 1:
      writeRegRF24L01(RF_SETUP, (readRegRF24L01(RF_SETUP) & 0xF7) | (1 << RF_DR_HIGH));
      return true;
    case 2:
      writeRegRF24L01(RF_SETUP, (readRegRF24L01(RF_SETUP) & 0xDF) | (1 << RF_DR_LOW));
      return true;
    default:
      return false;
  }
}

bool RF24L01::setTransmitPower(uint8_t tPow){
  if(tPow < 4){
    writeRegRF24L01(RF_SETUP, (readRegRF24L01(RF_SETUP) & 0xF9) | tPow);
    return true;
  }
  else{
    return false;
  }
}

bool RF24L01::hasReceiveData(){
  uint8_t status = getStatus();
  return ((status >> RX_DR));
}

void RF24L01::listenForTransmission(){
  writeRegRF24L01(CONFIG, 0xB);
  PORTB |= (1 << CE_PIN);
}
