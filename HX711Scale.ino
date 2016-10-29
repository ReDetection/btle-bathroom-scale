#include <SevSeg.h>
#include <HX711.h>
#include <avr/sleep.h>
#include <BTLE.h>
#include <RF24.h>

HX711 scale(7, 6);		// parameter "gain" is ommited; the default value 128 is used by the library
SevSeg sevseg;
RF24 radio(9,10);
BTLE btle(&radio);
unsigned long long lastChangedTime;
float lastWeight;

struct nrf_weight_data {
  int16_t service_uuid;
  int16_t value;
};
nrf_weight_data payload;

void splash() {
  for(byte i = 9; i > 0; i--) {
    sevseg.setNumber(i*1111, i % 4);
    unsigned long long nextFrame = millis() + 200;
    while(nextFrame > millis()) {
      sevseg.refreshDisplay();
    }
  }
}

void setup() {
  byte digitPins[] = {14, 17, 18, 8};
  byte segmentPins[] = {15, 19, 4, 21, 5, 16, 3, 20};
  sevseg.begin(COMMON_ANODE, 4, digitPins, segmentPins);
  sevseg.setBrightness(50);
  scale.read();
  btle.begin("Scale"); //8 chars max
  payload.service_uuid = 0x181D; // org.bluetooth.service.weight_scale
  scale.set_scale(27477.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  splash();
  scale.tare();				        // reset the scale to 0
//  Serial.begin(9600);
}

void powerDownCpu() {
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}

inline float ABS(float a) {
  return a >=0 ? a : -a;
}

void loop() {
  float kg = scale.get_units(1);
//  Serial.println(kg);
  sevseg.setNumber(kg, 1);
  payload.value = kg/0.005;

  for (byte i=0; i<3; i++) {
    unsigned long long nextAdvertisement = millis() + 100;
    while(nextAdvertisement > millis()) {
      sevseg.refreshDisplay();
    }
    
    btle.advertise(0x16, &payload, sizeof(payload));
    btle.hopChannel();
  }

  if (ABS(kg-lastWeight)>1.f) {
    lastWeight = kg;
    lastChangedTime = millis();
  }

  if (lastChangedTime + 15000 < millis()) {
    scale.power_down();
    radio.powerDown();
    powerDownCpu();
  }
  
}
