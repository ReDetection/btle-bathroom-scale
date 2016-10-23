#include <SevSeg.h>
#include <HX711.h>
#include <avr/sleep.h>

// HX711.DOUT	- pin #A1
// HX711.PD_SCK	- pin #A0

HX711 scale(7, 6);		// parameter "gain" is ommited; the default value 128 is used by the library
SevSeg sevseg;
unsigned long long lastChangedTime;
float lastWeight;


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

void powerDownCpu() {
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sleep_cpu();                            // sleep
}

inline float ABS(float a) {
  return a >=0 ? a : -a;
}

void loop() {
  float kg = scale.get_units(1);
//  Serial.println(kg);
  sevseg.setNumber(kg, 1);
  unsigned long long t = millis() + 1000;
  while(t > millis()) {
    sevseg.refreshDisplay();
  }

  if (ABS(kg-lastWeight)>1.f) {
    lastWeight = kg;
    lastChangedTime = millis();
  }

  if (lastChangedTime + 15000 < millis()) {
    scale.power_down();
    powerDownCpu();
  }
  
}
