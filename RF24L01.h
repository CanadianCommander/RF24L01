#ifndef RF24L01_H
#define RF24L01_H

#include <Arduino.h>

//SPI ----------------------------
extern void initSPI();
extern uint8_t transmitSPI(uint8_t msg);
//////////////////////////////////
// RF24L01 ----------------------
#ifndef CE_PIN
#warning "CE Pin not defined defaulting to .... PORTB1"
#define CE_PIN PORTB1
#endif

#ifndef CSN_PIN
#warning "CSN Pin not defined defaulting to .... PORTB2"
#define CSN_PIN PORTB2
#endif

namespace RF24L01 {

  extern void initRF24L01();
  extern void configureAsReceiver();
  extern uint8_t writeRegRF24L01(uint8_t addr, uint8_t reg);
  extern uint8_t writeRegRF24L01(uint8_t addr, uint8_t * data, uint8_t len);
  extern uint8_t writeTxPayload(uint8_t data);
  extern uint8_t writeTxPayload(uint8_t * data, uint8_t len);
  extern uint8_t writeRxPayload(uint8_t pipe, uint8_t data);
  extern uint8_t writeRxPayload(uint8_t pipe, uint8_t * data, uint8_t len);

  extern uint8_t readRegRF24L01(uint8_t addr);
  extern uint8_t readRxPayload(uint8_t * buffer, uint8_t len);

  extern uint8_t getStatus();
  extern uint8_t flushRreceiveBuffer();
  extern uint8_t flushTransmitBuffer();

  extern void powerUp();
  extern void powerDown();

  extern bool setTransmitAddress(uint8_t * addr, uint8_t len);
  extern bool setReceiveAddress(uint8_t * addr, uint8_t len);
  //NOTE when setting a pipe other than pipe 0 you can only set the LSBit of the address (len must == 1)
  extern bool setReceiveAddress(uint8_t pipe, uint8_t * addr, uint8_t len);
  extern bool setChannel(uint8_t channel);

  extern bool transmitMsg(uint8_t data);
  extern bool transmitMsg(uint8_t * data, uint8_t len);

  extern void getReceivedMsg(uint8_t * buffer, uint8_t len);
  extern void setResponseMsg(uint8_t pipe, uint8_t data);
  extern void setResponseMsg(uint8_t pipe, uint8_t * buffer, uint8_t len);

  extern uint8_t HasReceiveData();
  extern void listenForTransmission();
}
/////////////////////////////////

//commands
#define R_REGISTER(reg) reg & 0x1F
#define W_REGISTER(reg) (reg & 0x1F) | 0x20
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0
#define W_ACK_PAYLOAD(pipe) (pipe & 0x7) | 0xA8
#define FLUSH_RX     0xE2
#define FLUSH_TX     0xE1
#define NOP          0xFF

//register defines
#define CONFIG 0x00
#define MASK_RX_DR 6
#define MASK_TX_DS 5
#define MASK_MAX_RT 4
#define EN_CRC 3
#define CRCO 2
#define PWR_UP 1
#define PRIM_RX 0

#define EN_AA 0x01
#define ENAA_P5 5
#define ENAA_P4 4
#define ENAA_P3 3
#define ENAA_P2 2
#define ENAA_P1 1
#define ENAA_P0 0

#define EN_RXADDR 0x02
#define ERX_P5 5
#define ERX_P4 4
#define ERX_P3 3
#define ERX_P2 2
#define ERX_P1 1
#define ERX_P0 0

#define SETUP_AW 0x03
#define AW 0

#define SETUP_RETR 0x04
#define ARDa 4
#define ARC 0

#define RF_CH 0x05
// bit 0 - 6 frequency of operation

#define RF_SETUP 0x06
#define CONT_WAVE 7
#define RF_DR_LOW 5
#define PPL_LOCK 4
#define RF_DR_HIGH 3
#define RF_PWR 1

#define STATUS 0x07
#define RX_DR 6
#define TX_DS 5
#define MAX_RT 4
#define RX_P_NO 1
#define TX_FULL 0

#define OBSERVE_TX 0x08
#define PLOS_CNT 4
#define ARC_CNT 0

#define RPD 0x09
//bit 0 is reciver power detect??? HIGH when pwr > some thresh?

#define RX_ADDR_P0 0x0A
//address of pipe 0 (max 5 bytes)
#define RX_ADDR_P1 0x0B
//address of pipe 1 (max 5 bytes)
#define RX_ADDR_P2 0x0C
#define RX_ADDR_P3 0x0D
#define RX_ADDR_P4 0x0E
#define RX_ADDR_P5 0x0F

#define TX_ADDR 0x10
// transmit address max size 5 bytes

#define RX_PW_P0 0x11
// number of bytes in RX payload in pipe 0 Max 32
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16

#define FIFO_STATUS 0x17
#define TX_REUSE_FIFO 6
#define TX_FULL_FIFO 5
#define TX_EMPTY_FIFO 4
#define RX_FULL_FIFO 1
#define RX_EMPTY_FIFO 0

#define DYNPD 0x1C
#define DPL_P5 5
#define DPL_P4 4
#define DPL_P3 3
#define DPL_P2 2
#define DPL_P1 1
#define DPL_P0 0

#define FEATURE 0x1D
#define EN_DPL 2
#define EN_ACK_PAY 1
#define EN_DYN_ACK 0

#endif // RF24L01_H
