/* rawSend.ino Example sketch for IRLib2
 *  Illustrates how to send a code Using raw timings which were captured
 *  from the "rawRecv.ino" sample sketch.  Load that sketch and
 *  capture the values. They will print in the serial monitor. Then you
 *  cut and paste that output into the appropriate section below.
 */
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender
#include <dht.h>

#define dht_apin A0 // Analog Pin sensor is connected to
 
dht DHT;

IRsendRaw mySender;

void setup() {
  Serial.begin(9600);
  delay(2000); 
  while (!Serial); //delay for Leonardo
  Serial.println("DHT11 Humidity & temperature Sensor\n\n");
}
/* Cut and paste the output from "rawRecv.ino" below here. It will 
 * consist of a #define RAW_DATA_LEN statement and an array definition
 * beginning with "uint16_t rawData[RAW_DATA_LEN]= {…" and concludes
 * with "…,1000};"
 */
#define RAW_DATA_LEN 244
uint16_t rawDataOff[RAW_DATA_LEN]={
  8370, 4294, 466, 634, 466, 1730, 466, 1738, 
  466, 626, 466, 1730, 466, 634, 466, 1730, 
  466, 642, 466, 626, 462, 634, 466, 1730, 
  466, 1742, 466, 622, 466, 1734, 462, 1742, 
  466, 626, 462, 646, 462, 626, 466, 642, 
  466, 626, 462, 646, 462, 626, 466, 642, 
  466, 634, 462, 638, 462, 634, 466, 634, 
  466, 634, 466, 634, 462, 634, 466, 634, 
  466, 634, 466, 626, 462, 642, 466, 626, 
  466, 642, 466, 626, 462, 1742, 466, 634, 
  466, 634, 466, 626, 462, 646, 462, 626, 
  466, 642, 466, 626, 466, 634, 462, 1734, 
  466, 1742, 466, 630, 466, 634, 494, 606, 
  466, 634, 466, 634, 490, 610, 462, 634, 
  494, 606, 494, 598, 494, 614, 494, 594, 
  494, 614, 494, 598, 494, 614, 490, 602, 
  490, 618, 490, 598, 466, 646, 490, 598, 
  494, 614, 494, 598, 494, 614, 490, 602, 
  490, 618, 490, 606, 494, 606, 494, 606, 
  494, 606, 490, 610, 490, 606, 466, 634, 
  494, 598, 494, 614, 494, 598, 494, 614, 
  494, 598, 490, 614, 494, 598, 494, 614, 
  494, 598, 494, 614, 494, 598, 490, 618, 
  494, 594, 494, 614, 494, 598, 494, 614, 
  494, 606, 494, 606, 490, 610, 490, 610, 
  462, 638, 490, 606, 494, 606, 494, 606, 
  494, 606, 490, 602, 490, 618, 490, 598, 
  470, 638, 466, 626, 466, 642, 466, 626, 
  490, 610, 490, 1706, 466, 1742, 494, 598, 
  490, 1706, 494, 606, 494, 1706, 466, 642, 
  466, 622, 466, 1000};


#define RAW_DATA_LEN 244
uint16_t rawDataOn[RAW_DATA_LEN]={
  8394, 4294, 466, 614, 486, 1730, 466, 1738, 
  466, 626, 466, 1730, 466, 634, 466, 1730, 
  466, 638, 470, 622, 466, 634, 466, 1730, 
  466, 1738, 466, 626, 466, 1730, 466, 1738, 
  470, 622, 466, 642, 466, 622, 466, 642, 
  466, 626, 466, 642, 466, 622, 466, 642, 
  466, 634, 466, 630, 466, 634, 466, 634, 
  466, 630, 466, 634, 466, 634, 466, 634, 
  466, 630, 466, 626, 466, 642, 466, 622, 
  466, 642, 466, 626, 466, 1738, 466, 634, 
  466, 630, 466, 606, 486, 622, 486, 622, 
  470, 618, 486, 626, 466, 642, 466, 606, 
  486, 618, 486, 626, 466, 642, 466, 602, 
  486, 642, 466, 606, 486, 642, 466, 622, 
  466, 642, 466, 634, 466, 614, 486, 630, 
  466, 614, 486, 634, 466, 630, 470, 630, 
  466, 626, 466, 642, 466, 626, 462, 622, 
  486, 626, 466, 622, 486, 602, 486, 642, 
  466, 626, 466, 642, 466, 622, 466, 642, 
  466, 626, 466, 638, 470, 622, 466, 642, 
  466, 614, 486, 634, 466, 634, 462, 634, 
  466, 634, 466, 634, 466, 630, 466, 618, 
  482, 634, 466, 626, 466, 642, 466, 622, 
  466, 642, 466, 626, 466, 642, 466, 622, 
  466, 642, 466, 626, 466, 642, 466, 626, 
  462, 622, 486, 626, 466, 642, 466, 626, 
  466, 642, 466, 630, 466, 634, 466, 634, 
  466, 630, 470, 630, 470, 610, 486, 634, 
  466, 626, 466, 1710, 486, 1734, 466, 1730, 
  466, 1714, 486, 1738, 466, 626, 466, 642, 
  466, 630, 470, 1000};




/*
 * Cut-and-paste into the area above.
 */
String offstatus = "";
   
void loop() {

  DHT.read11(dht_apin);
  float temp = DHT.temperature;
  Serial.print("temperature = ");
  Serial.print(temp); 
  Serial.println(" C");
  if (temp > 27 && (offstatus == "off" || offstatus == "")) {
    mySender.send(rawDataOn,244,36);//Pass the buffer,length, optionally frequency
    Serial.println(F("AC Switched On"));
    offstatus = "on";
  }
  else if (temp < 26 && (offstatus == "on" || offstatus == "")) { 
    offstatus = "off";
    mySender.send(rawDataOff,244,36);//Pass the buffer,length, optionally frequency
    Serial.println(F("AC Switched Off"));
  }
  Serial.println(offstatus);
  delay(5000);
}
