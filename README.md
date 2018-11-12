# RF24L01 library.
this library allows light weight & easy control of the RF24L01 from any Arduino MCU. All while using
0 bytes of SRAM!
### documentation
see [docs](https://canadiancommander.github.io/RF24L01/)
### Install
simple, just copy N paste the library folder in to where ever you store all your other Arduino libraries.
for the Arduino IDE this is most likely under: $HOME/Documents/Arduino/libraries/ or /usr/share/arduino/libraries
### examples
full examples in example folder.

Sender:
```
void setup(){
  Serial.begin(9600);

  initRF24L01();
  setTransmitAddress(myAddr,5);

  setChannel(2);
  setDataRate(1);//2 Mbps
  setRetransmitTime(0xF);// 4000 nano
}

uint8_t dataBuff[33];
void loop(){
  delay(1000);
  transmitMsg((uint8_t*)"HELLO WORLD", 12);
}
```
Receiver:
```
void setup(){
  Serial.begin(9600);

  initRF24L01();
  setReceiveAddress(myAddr, 5);
  configureAsReceiver();

  setChannel(2);
  setDataRate(1);// 2 Mbps
  setRetransmitTime(0xF);// 4000 nano
}

uint8_t dataBuff[33];
void loop(){
  delay(1000);
  if(hasReceiveData()){
    getReceivedMsg(dataBuff,33);
    Serial.print("I have mail: ");
    Serial.print((char*)dataBuff);
    Serial.print("\n");
  }
}
```
and its just that easy!
