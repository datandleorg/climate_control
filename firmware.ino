#include <Wire.h>
#include <IRLibSendBase.h>    // sending base classes
#include <IRLib_HashRaw.h>    //We need this for IRsendRaw
#include <IRLibRecvPCI.h> 
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include "DHT.h"
// 1-10
// 11-20
// 21 22 - high low
// 100 - 1000
// 1000 - off code
#define RST_PIN -1
#define EEPROM_I2C_ADDRESS 0x50

DHT dht(7, DHT11);
SSD1306AsciiAvrI2c oled;

IRsendRaw mySender;
IRrecvPCI myReceiver(2);//pin number for the receiver

const byte SET_BTN_PIN_1 = 8; 
const byte BUTTON_PIN_INC = 9; 
const byte BUTTON_PIN_DEC = 12;  
const byte RESET_BTN_PIN_1 = 10; 

byte currentButtonState_1; // the current state of button
byte lastButtonState_1;    // the previous state of button
byte currentButtonState_2; // the current state of button
byte lastButtonState_2;    // the previous state of button
byte currentButtonState_3; // the current state of button
byte lastButtonState_3;    // the previous state of button
byte currentButtonState_4; // the current state of button
byte lastButtonState_4;    // the previous state of button

byte battery = 67;    // the previous state of button
boolean blinkStatus_1 = true;
boolean blinkStatus_2 = false;
byte screen = 1;
byte high = 28;
byte low = 23;
const unsigned long readingEvent = 5000;
unsigned long previousTime = 0;
float temp;
int currAddr[2];
byte offstatus;

void setup(){
  Wire.begin();
  Serial.begin(9600); 

  #if RST_PIN >= 0
    oled.begin(&Adafruit128x64, 0x3C, RST_PIN);
  #else 
    oled.begin(&Adafruit128x64, 0x3C);
  #endif 
    oled.setFont(Adafruit5x7);

  oled.clear();
  oled.set2X();
  oled.setCursor(12,2);
  oled.print(F("*Climate*"));
  oled.setCursor(12,4);
  oled.print(F("*Control*"));
  
  myReceiver.enableIRIn(); // Start the receiver

  pinMode(SET_BTN_PIN_1, INPUT_PULLUP); 
  pinMode(BUTTON_PIN_INC, INPUT_PULLUP); 
  pinMode(BUTTON_PIN_DEC, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(RESET_BTN_PIN_1, INPUT_PULLUP); // set arduino pin to input pull-up mode
  
  currentButtonState_1 = digitalRead(SET_BTN_PIN_1);
  currentButtonState_2 = digitalRead(BUTTON_PIN_INC);
  currentButtonState_3 = digitalRead(BUTTON_PIN_DEC);
  currentButtonState_4 = digitalRead(RESET_BTN_PIN_1);

  high = readEEPROM(EEPROM_I2C_ADDRESS, 21);
  low = readEEPROM(EEPROM_I2C_ADDRESS, 22); 

  dht.begin();
  delay(500);
  temp = dht.readTemperature();
  oled.clear();
  getAddr(1);
}

void loop() { 
      lastButtonState_1    = currentButtonState_1;      // save the last state
      currentButtonState_1 = digitalRead(SET_BTN_PIN_1); // read new state
      lastButtonState_2   = currentButtonState_2;      // save the last state
      currentButtonState_2 = digitalRead(BUTTON_PIN_INC); // read new state
      lastButtonState_3   = currentButtonState_3;      // save the last state
      currentButtonState_3 = digitalRead(BUTTON_PIN_DEC); // read new state
      lastButtonState_4   = currentButtonState_4;      // save the last state
      currentButtonState_4 = digitalRead(RESET_BTN_PIN_1); // read new state

      unsigned long currentTime = millis();
      
      if(lastButtonState_1 == HIGH && currentButtonState_1 == LOW) {
        incScreen();
      }
      
      if(lastButtonState_2 == HIGH && currentButtonState_2 == LOW) {
        if(blinkStatus_1){
             if(high >= 50){
                 high = 1;
              }else{
                 high = high + 1;
              }
        } else {
             if(low >= 50){
                 low = 1;
              }else{
                 low = low + 1;
              }
        }
        if (blinkStatus_1) {
            writeEEPROM(EEPROM_I2C_ADDRESS,21,high);
        } else {
            writeEEPROM(EEPROM_I2C_ADDRESS,22,low);
        }
      }
      
      if(lastButtonState_3 == HIGH && currentButtonState_3 == LOW) {
        if(blinkStatus_1){
             if(high <= 1){
                 high = 1;
              }else{
                 high = high - 1;
              }
        } else {
             if(low <= 1){
                 low = 1;
              }else{
                 low = low - 1;
              }
        }
        if (blinkStatus_1) {
            writeEEPROM(EEPROM_I2C_ADDRESS,21,high);
        } else {
            writeEEPROM(EEPROM_I2C_ADDRESS,22,low);
        }
      }

      if(lastButtonState_4 == HIGH && currentButtonState_4 == LOW) {
        reset();
      } 

     if(screen == 1) {
        oled.set1X();
        oled.setCursor(0,1);
        oled.print(F("Power:"));
        oled.setCursor(40,1);
        oled.print(battery);
        oled.print(F("%"));
        oled.setCursor(90,1);
        oled.print(F("status"));

        oled.set2X();
        oled.setCursor(0,3);
        oled.print(temp);
        oled.set1X();
        oled.print("o");
        oled.set2X();
        oled.print("C");
        
        oled.set2X();
        oled.setCursor(96,3);
        oled.print(offstatus ? offstatus == 0 ? "OFF" : "ON" : "--");

        oled.set1X();
        oled.setCursor(0,5);
        oled.print(F("----------------------"));
        
        if(currAddr[1] > 0){
          oled.setCursor(0,6);
          oled.print(F("High: "));
          oled.print(high);
          oled.print(F(" C"));

          oled.setCursor(0,7);
          oled.print(F("Low: "));
          oled.print(low);
          oled.print(F(" C"));

          if (currentTime - previousTime >= readingEvent) {
            temp = dht.readTemperature();
            if (isnan(temp)) {
              Serial.println(F("Failed to read from DHT sensor!"));
              return;
            }

            if (temp > high && (offstatus == 0 || !offstatus)) {
                getAddr(1);
                uint16_t code_1[currAddr[1]+1];
                readIntArrayFromEEPROM(currAddr[0], code_1, currAddr[1] + 1);
                mySender.send(code_1,currAddr[1]+1,36);//Pass the buffer,length, optionally frequency
                offstatus = 1;

                Serial.println("AC Switched On");
                Serial.println(offstatus);

                oled.clear();
                oled.setCursor(0,7);
                oled.print(F("Turning ON "));
                delay(1000);
                oled.clear();
                
             }
              else if (temp < low && (offstatus == 1 || !offstatus)) { 
                getAddr(0);
                uint16_t code_0[currAddr[1]+1];
                readIntArrayFromEEPROM(currAddr[0], code_0, currAddr[1]+1);
                mySender.send(code_0,currAddr[1]+1,36);//Pass the buffer,length, optionally frequency
                offstatus = 0;
                
                Serial.println("AC Switched Off");
                Serial.println(offstatus);

                oled.clear();
                oled.setCursor(0,7);
                oled.print(F("Turning OFF"));
                delay(1000);
                oled.clear();
             } 
               
            previousTime = currentTime;
          }
        }else{
          oled.setCursor(0,6);
          oled.print(F("Setup is pending"));
          
          oled.setCursor(0,7);
          oled.print(F("Click on SET button"));
        }
     }

    if(screen == 2 ) {
      oled.setCursor(0,2);
      oled.print(F("Pls direct your remote"));
      oled.setCursor(0,3);
      oled.print(F("towards the device"));
      oled.setCursor(0,4);
      oled.print(F("and Power ON your AC"));
      recieveCode(true, 1);
     }
     
    if(screen == 3) {
      oled.setCursor(0,2);
      oled.print(F("Pls direct your remote"));
      oled.setCursor(0,3);
      oled.print(F("towards the device"));
      oled.setCursor(0,4);
      oled.print(F("and Power OFF your AC"));
      recieveCode(true, 0);
   }
   
   if(screen == 4) {
       oled.set1X();
       oled.setCursor(0,1);
       oled.print(F("High"));
       oled.setCursor(90,1);
       oled.print(F("Low"));

       oled.set2X();
       oled.setCursor(0,4);

        if(blinkStatus_1){
          oled.setInvertMode(true);
          oled.print(high);
          oled.setInvertMode(false);
        }else{
          oled.print(high);
        }

       oled.setCursor(90,4);
        if(blinkStatus_2){
          oled.setInvertMode(true);
          oled.print(low);
          oled.setInvertMode(false);
        }else{
          oled.print(low);
        }   
   }
}

void recieveCode(boolean recieverActive, int type){
  if(recieverActive){
     if (myReceiver.getResults()) { 
      if (recvGlobal.recvLength - 1 > 30) {
        int rawCode[recvGlobal.recvLength - 1];
        for(bufIndex_t i=1;i<recvGlobal.recvLength;i++) {
          rawCode[i-1] = (int) recvGlobal.recvBuffer[i];
        }
        rawCode[recvGlobal.recvLength - 1] = 1000;
        getAddr(type);
        writeIntArrayIntoEEPROM(currAddr[0], rawCode, recvGlobal.recvLength);
        recordAddressChannel(type,recvGlobal.recvLength - 1, currAddr[0]);
      }
      myReceiver.enableIRIn();      
    }
  }
}


void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) 
{
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(5);
}

byte readEEPROM(int deviceaddress, unsigned int eeaddress ) 
{
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

void writeIntArrayIntoEEPROM(int address, int numbers[], int arraySize)
{
  int addressIndex = address;
  for (int i = 0; i < arraySize; i++) 
  {
    writeEEPROM(EEPROM_I2C_ADDRESS, addressIndex, numbers[i] >> 8);
    writeEEPROM(EEPROM_I2C_ADDRESS, addressIndex + 1, numbers[i] & 0xFF);
    addressIndex += 2;
  }
}
void readIntArrayFromEEPROM(int address, int numbers[], int arraySize)
{    
  int addressIndex = address;
  for (int i = 0; i < arraySize; i++)
  {
    numbers[i] = (readEEPROM(EEPROM_I2C_ADDRESS, addressIndex) << 8) + readEEPROM(EEPROM_I2C_ADDRESS, addressIndex + 1);
    addressIndex += 2;
  }
}

void getAddr(int type){
  readIntArrayFromEEPROM(type == 1 ? 1 : 11, currAddr,2);

  if (currAddr[0] > 0){
  } else {
    if (type == 1){
      currAddr[0]=100;
    } else {
      currAddr[0]=1000;
    }
  }
}

void recordAddressChannel(int type, int onAddrLen, int startAddr){
   int channelAddr = type == 1 ? 1 : 11;
   int rcd[2] = { startAddr, onAddrLen };
   writeIntArrayIntoEEPROM(channelAddr, rcd, 2);
   oled.clear();
   oled.setCursor(16,2);
   oled.print(type);
   oled.setCursor(0,3);
   oled.print(F("code recorded"));
   delay(1000);
   incScreen();
   recieveCode(false, 1);
}

void incScreen(){
    oled.clear();
    if(screen >= 4){
      if (screen == 4){
        if(blinkStatus_1){
          blinkStatus_1 = false;
          blinkStatus_2 = true;
        } else {
          screen = 1;
          blinkStatus_1 = true;
          blinkStatus_2 = false;
        }
      }
    }else{
      screen = screen + 1;
    }
}

void reset(){
  offstatus = NULL;
}
