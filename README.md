# RF24L01 library.
this library allows light weight & easy control of the RF24L01 from any Arduino MCU. All while using
0 bytes of SRAM!
### documentation
see [docs](https://canadiancommander.github.io/RF24L01/)
### Install
simple, just copy N paste the library folder in to where ever you store all your other Arduino libraries.
for the Arduino IDE this is most likely under: $HOME/Documents/Arduino/libraries/
### examples
full examples in example folder.

Sender:
```
void setup(){
  initRF24L01();
  setTransmitAddress(rcvAddr,5);
}

//transmit hello world every second.
void loop(){
  transmitMsg((uint8_t*)"HELLO WORLD", 12);
  delay(1000);
}
```
Receiver:
```
void setup(){
  initRF24L01();
  setReceiveAddress(rcvAddr, 5);
  configureAsReceiver();
}

//wait for msg
void loop(){
  delay(1000);
  if(hasReceiveData()){
    getReceivedMsg(dataBuff,33);
    Serial.print((char*)dataBuff);
  }
}
```
and its just that easy! 
