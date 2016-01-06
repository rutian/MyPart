#include <Wire.h>

#include "TLC59108.h"

#define HW_RESET_PIN 2
#define I2C_ADDR TLC59108::I2C_ADDR::SUB4

TLC59108 leds(I2C_ADDR);

void setup() {
  Wire.begin();
  leds.init(HW_RESET_PIN);
  leds.setLedOutputMode(TLC59108::LED_MODE::PWM_IND);
}

void loop(){
  sweep();
}

void sweep() {
  byte pwm;

  for(pwm = 0; pwm < 0xff; pwm++) {
    leds.setAllBrightness(pwm);
    delay(10);
  }

  for(pwm = 0xfe; pwm < 0xff; pwm--) {
    leds.setAllBrightness(pwm);
    delay(10);
  }
  
  for(byte channel = 0; channel < 8; channel++)
    for(pwm = 0; pwm < 0xff; pwm++) {
      leds.setBrightness(channel, pwm);
      delay(10);
    }
    
  for(byte channel = 0; channel < 8; channel++)
    for(pwm = 0xfe; pwm < 0xff; pwm--) {
      leds.setBrightness(channel, pwm);
      delay(10);
    }
}



