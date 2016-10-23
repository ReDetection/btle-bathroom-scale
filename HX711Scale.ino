#include <LowPower.h>
#include <SevSeg.h>
#include <HX711.h>

// HX711.DOUT	- pin #A1
// HX711.PD_SCK	- pin #A0

HX711 scale(7, 6);		// parameter "gain" is ommited; the default value 128 is used by the library
SevSeg sevseg;
unsigned long long lastChangeTime;
float lastRememberedWeight;


void setup() {
  byte digitPins[] = {14, 17, 18, 8};
  byte segmentPins[] = {15, 19, 4, 21, 5, 16, 3, 20};
  sevseg.begin(COMMON_ANODE, 4, digitPins, segmentPins);
  sevseg.setBrightness(50);
  scale.read();
  scale.set_scale(27477.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0
//  Serial.begin(9600);
}

void loop() {
  float kg = scale.get_units(1);
//  Serial.println(kg);
  sevseg.setNumber(kg, 1);
  unsigned long long t = millis() + 1000;
  while(t > millis()) {
    sevseg.refreshDisplay();
  }
  
//  scale.power_down();             // put the ADC in sleep mode
//  delay(5000);
//  scale.power_up();
}
