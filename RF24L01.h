/** \mainpage
  documentation here: RF24L01
*/
#ifndef RF24L01_H
#define RF24L01_H

#include <Arduino.h>

// RF24L01 ----------------------
#ifndef CE_PIN
#warning "CE Pin not defined defaulting to .... PORTB1"
#define CE_PIN PORTB1
#endif

#ifndef CSN_PIN
#warning "CSN Pin not defined defaulting to .... PORTB2"
#define CSN_PIN PORTB2
#endif

/**
  primary namespace for the RF24L01 Library
*/
namespace RF24L01 {

  /**
    initialize the RF24L01 radio. The randio must be connected to the SPI pins of your board
    and, CSN_PIN + CE_PIN must be set.
  */
  extern void initRF24L01();
  /**
    configure the RF24L01 radio as a "Primary Receiver". This means that you can
    only "talk" when spoken to. Think of it like SPI. you will use  setResponseMsg() to send these messages
    @see setResponseMsg
  */
  extern void configureAsReceiver();

  //low level stuff
  extern uint8_t writeRegRF24L01(uint8_t addr, uint8_t reg);
  extern uint8_t writeRegRF24L01(uint8_t addr, uint8_t * data, uint8_t len);
  extern uint8_t writeTxPayload(uint8_t data);
  extern uint8_t writeTxPayload(uint8_t * data, uint8_t len);
  extern uint8_t writeRxPayload(uint8_t pipe, uint8_t data);
  extern uint8_t writeRxPayload(uint8_t pipe, uint8_t * data, uint8_t len);

  extern uint8_t readRegRF24L01(uint8_t addr);
  extern uint8_t readRxPayload(uint8_t * buffer, uint8_t len);

  /**
    get the RF24L01 STATUS register
    @returns STATUS register 0-7 bit [TX_FULL,RX_P_NO,RX_P_NO,RX_P_NO,MAX_RT,TX_DS,RX_DR,Reserved]
    TX_FULL is set when the tx buffer is full. RX_P_NO is the Rx pipe number on which the next call to
    getReceiveMsg will return data from (handy for multi per communication). MAX_RT indicates a transmission timeout
    this flag is cleared by transmitMsg for you, said call will return false instead. TX_DS indicates that
    the transmission was successfull, again transmitMsg will handle it for you. RX_DR indicates that there is
    Rx FIFO data ready to be read.
  */
  extern uint8_t getStatus();
  /**
    flush the Rx FIFO. handy if its cloged up with a bunch of shit.
    @return STATUS register
    @see getStatus
  */
  extern uint8_t flushRreceiveBuffer();
  /**
    flush the Tx FIFO. handy if its cloged up with a bunch of shit.
    @return STATUS register
    @see getStatus
  */
  extern uint8_t flushTransmitBuffer();

  /**
    power up the RF24L01 module
  */
  extern void powerUp();
  /**
    power down the RF24L01 module (to save power ofc)
  */
  extern void powerDown();

  /**
    set the address to which you will transmit. NOTE, will overwrite RX_ADDR_P0 for ack message handling.
    so just keep that in mind if you plan to use it latter.
    @param addr the address to which transmissions are sent
    @param len the length of the address < 5 bytes
    @return ture on success
  */
  extern bool setTransmitAddress(uint8_t * addr, uint8_t len);

  /**
    set the address of a pipe0-5. the transmiter must target this address for message transmission to work.
    pipe zero is set to addr, RX_ADDR_P1 - RX_ADDR_P5 are set to increments of RX_ADDR_P0
    @param addr address of pipe 0
    @param len length of the address < 5 bytes
    @return true on success
  */
  extern bool setReceiveAddress(uint8_t * addr, uint8_t len);

  /**
    set the address of a pipe. the transmiter must target this address for message transmission to work.
    @param pipe (RX_ADDR_P0 - RX_ADDR_P5) NOTE, when setting a pipe other than pipe 0 you can only set the LSBit of the address (len must == 1)
    @param addr address
    @param len length of address < 5 bytes
    @return true on succes
  */
  extern bool setReceiveAddress(uint8_t pipe, uint8_t * addr, uint8_t len);
  /**
    set transmission channel. NOTE, if in 2Mbps mode (the default) you should leave a 1 channel gap between
    channels in multi transmiter scenarios.
    @param channel 0 - 128 channel select.
    @return true on success
  */
  extern bool setChannel(uint8_t channel);
  /**
  set the amount of delay before a packet is retransmited
  @param rTime
  setps of 250 nano seconds starting at:
  0000 = disabled,
  0001 = 250 (in most cases wont work due to 2.4 Ghz clutter)
  0010 = 500
  ...........
   1111 = 4000
   @return true on success
  */
  extern bool setRetransmitTime(uint8_t rTime);
  /**
    set the number of retransmission before a timeout.
    @param rCount a number less than 0x10.
    @return true on success
    @see transmitMsg
  */
  extern bool setRetransmitCount(uint8_t rCount);
  /**
    set data rate. must match the data rate of the RF module you are trying to talk to.
    @param dRate 0 = 1 Mbps, 1 = 2 Mbps, 2 = 250 kbps.
    @return true on success
  */
  extern bool setDataRate(uint8_t dRate);
  /**
    set transmission power.... POWER!
    @param tPow: 0 = -18 dBm, 1 = -12 dBm, 2 = -6 dBm, 3 = 0 dBm.
    @return true on success
  */
  extern bool setTransmitPower(uint8_t tPow);

  /**
    transmit a message (Tx FIFO must not be full)
    @param data a one byte message to transmit
    @return true if message transmission success, false on timeout.
    @see setTransmitAddress
    @see setChannel
    @see setTransmitPower
    @see setDataRate
    @see setRetransmitTime
    @see setRetransmitCount
    @see getStatus
  */
  extern bool transmitMsg(uint8_t data);
  /**
    transmit a message (Tx FIFO must not be full)
    @param data buffer to transmit
    @param len the length of the data buffer, must not exceed 32 bytes
    @return true if message transmission success, false on timeout.
    @see setTransmitAddress
    @see setChannel
    @see setTransmitPower
    @see setDataRate
    @see setRetransmitTime
    @see setRetransmitCount
    @see getStatus
  */
  extern bool transmitMsg(uint8_t * data, uint8_t len);

  /**
    read the next received message in the Rx FIFO.
    @param buffer buffer to receive the data in the Rx FIFO
    @param len the length of the buffer. bytes are only read up to len.
           also if len is higher than the amount of data in the Rx FIFO
           garbage data will be returned and it is up to the programmer to
           ignore it.
    @see  hasReceiveData
  */
  extern void getReceivedMsg(uint8_t * buffer, uint8_t len);

  /**
    set the message you whish to send in responce to a message from a transmiting RF24L01.
    This method will return immediately! this does not mean the message was sent!
    @param pipe the Rx pipe on which to send this message (RX_ADDR_P0 - RX_ADDR_P5)
    @param data one byte of data to send
    @see configureAsReceiver
    @see setResponseMsg
  */
  extern void setResponseMsg(uint8_t pipe, uint8_t data);
  /**
    multi byte version of setResponseMsg.
    @param pipe the Rx pipe on which to send this message (RX_ADDR_P0 - RX_ADDR_P5)
    @param data buffer to send. NOTE: must not exceed 32 bytes!
    @param len the length of the passed buffer
    @see setResponseMsg
  */
  extern void setResponseMsg(uint8_t pipe, uint8_t * buffer, uint8_t len);

  /**
    check if there is pending data in the Rx FIFO
    @return true if there is data in the Rx FIFO.
    @see getReceivedMsg
  */
  extern bool hasReceiveData();

  /**
    start activly listening for a transmision on all pipes
    @see setReceiveAddress
  */
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
