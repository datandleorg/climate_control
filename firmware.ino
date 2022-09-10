#include <Wire.h>
#include <EEPROM.h>
#include <IRremote.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1
SSD1306AsciiAvrI2c oled;

int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;

const uint16_t variableName[] PROGMEM = {}; // use this form
const byte BUTTON_PIN_1 = 5; // set
const byte BUTTON_PIN_INC = 6; // set
const byte BUTTON_PIN_DEC = 10; // set
byte currentButtonState_1; // the current state of button
byte lastButtonState_1;    // the previous state of button
byte currentButtonState_2; // the current state of button
byte lastButtonState_2;    // the previous state of button
byte currentButtonState_3; // the current state of button
byte lastButtonState_3;    // the previous state of button
byte battery = 67;    // the previous state of button
boolean invert = false;
boolean blinkStatus_1 = false;
boolean blinkStatus_2 = true;
byte screen = 1;
byte high = 28;
byte low = 23;
const unsigned long readingEvent = 5000;
unsigned long previousTime = 0;
float temp;
void setup(){
  Serial.begin(9600);                // initialize serial
  #if RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
  #else // RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
  #endif // RST_PIN >= 0
    oled.setFont(Adafruit5x7);

  irrecv.enableIRIn(); // Start the receiver
  oled.clear();
  oled.set2X();
  oled.setCursor(12,2);
  oled.print(F("*Climate*"));
  oled.setCursor(12,4);
  oled.print(F("*Control*"));

  pinMode(BUTTON_PIN_1, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(BUTTON_PIN_INC, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(BUTTON_PIN_DEC, INPUT_PULLUP); // set arduino pin to input pull-up mode
  currentButtonState_1 = digitalRead(BUTTON_PIN_1);
  currentButtonState_2 = digitalRead(BUTTON_PIN_INC);
  currentButtonState_3 = digitalRead(BUTTON_PIN_DEC);
  Serial.println(currentButtonState_1);
  dht.begin();
  delay(500);
  temp = dht.readTemperature();
  oled.clear();
  Serial.println(EEPROM.length());
}

void loop() { 
      lastButtonState_1    = currentButtonState_1;      // save the last state
      currentButtonState_1 = digitalRead(BUTTON_PIN_1); // read new state
      lastButtonState_2   = currentButtonState_2;      // save the last state
      currentButtonState_2 = digitalRead(BUTTON_PIN_INC); // read new state
      lastButtonState_3    = currentButtonState_3;      // save the last state
      currentButtonState_3 = digitalRead(BUTTON_PIN_DEC); // read new state
      unsigned long currentTime = millis();

      if (currentTime - previousTime >= readingEvent) {
            temp = dht.readTemperature();
            if (isnan(temp)) {
              Serial.println(F("Failed to read from DHT sensor!"));
              return;
            }
            previousTime = currentTime;
      }
      

     
      if(lastButtonState_1 == HIGH && currentButtonState_1 == LOW) {
        oled.clear();
        if(screen >= 4){
          screen = 1;
        }else{
          screen = screen + 1;
        }
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
        oled.setCursor((EEPROM.read(1) ? 90 : 96),3);
        oled.print(EEPROM.read(1) ? F("OFF") : F("--"));

        oled.set1X();
        oled.setCursor(0,5);
        oled.print(F("----------------------"));
        
        if(EEPROM.read(50)){
          oled.setCursor(0,6);
          oled.print(F("High: "));
          oled.print(F("25 C"));
          
          oled.setCursor(0,7);
          oled.print(F("Low: "));
          oled.print(F("28 C"));
        }else{
          oled.setCursor(0,6);
          oled.print(F("Setup is pending"));
          
          oled.setCursor(0,7);
          oled.print(F("Click on SET button"));
        }
     }

     
    if(screen == 2 ) {
      if(!EEPROM.read(50)){
        oled.setCursor(0,2);
        oled.print(F("Pls direct your remote"));
        oled.setCursor(0,3);
        oled.print(F("towards the device"));
        oled.setCursor(0,4);
        oled.print(F("and Power ON your AC"));

        initRead();
      }else {

        //Serial.println(EEPROM.read(50));

        oled.setCursor(16,2);
        oled.println(F("ON Signal recieved"));
        oled.setCursor(16,3);
        oled.println(readStringFromEEPROM(50));
      }
     }
     
    if(screen == 3) {
      if(!EEPROM.read(51)){
        oled.setCursor(0,2);
        oled.print(F("Pls direct your remote"));
        oled.setCursor(0,3);
        oled.print(F("towards the device"));
        oled.setCursor(0,4);
        oled.print(F("and Power OFF your AC"));
      }else {
        oled.setCursor(16,2);
        oled.print(F("OFF Signal recieved"));
      }
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

void initRead(){
  if (irrecv.decode(&results)) {
    Serial.println(results.value, DEC);
    dump(&results);
    irrecv.resume(); // Receive the next value
  }
}



void dump(decode_results *results) {
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print(F("Unknown encoding: "));
  }
  else if (results->decode_type == NEC) {
    Serial.print(F("Decoded NEC: "));
  }
  else if (results->decode_type == SONY) {
    Serial.print(F("Decoded SONY: "));
  }
  else if (results->decode_type == RC5) {
    Serial.print(F("Decoded RC5: "));
  }
  else if (results->decode_type == RC6) {
    Serial.print(F("Decoded RC6: "));
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print(F("Decoded PANASONIC - Address: "));
    Serial.print(results->address, HEX);
    Serial.print(F(" Value: "));
  }
  else if (results->decode_type == LG) {
    Serial.print(F("Decoded LG: "));
  }
  else if (results->decode_type == JVC) {
    Serial.print(F("Decoded JVC: "));
  }
//  else if (results->decode_type == AIWA_RC_T501) {
//    Serial.print("Decoded AIWA RC T501: ");
//  }
  else if (results->decode_type == WHYNTER) {
    Serial.print(F("Decoded Whynter: "));
  }
  Serial.print(results->value, HEX);
  Serial.print(F(" ("));
  Serial.print(results->bits, DEC);
  Serial.println(F(" bits)"));
  Serial.print(F("Raw ("));
  Serial.print(count, DEC);
  Serial.print("): ");

        Serial.println("-------");
        Serial.println(results->value, HEX);

   writeStringToEEPROM(50, String(results->value, HEX));
   
   oled.clear();
  for (int i = 1; i < count; i++) {
    if (i & 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.write('-');
      Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
  
}

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  Serial.println("*******");
  Serial.println(len);
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  // data[newStrLen] = '\ 0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  return String(data);
}
