/*****************************************************************
*
* Simple code used to test particle counter in chamber
* 
* normally in low power sleep mode
* 
* If woken by a short press: 
* (1) blink green light
* (2) sample particles for 45 seconds
* (3) sample temperature and humidty
* (4) go back to sleep
*
* If woken by a long press:
* (1) blink blue light
* (2) broadcast over GZLL the last sample (particles/temp/humidity)
* (3) go back to sleep
*
*
* Rundong Tian 
* 2016
*
******************************************************************/


struct sensor_data{
  int small;
  int large;
  float humidity;
  float temperature;
}; 

struct sensor_data data;


            
// rfduino specific
#include <RFduinoGZLL.h>
#include <SPI.h>  // need to modify board definition to change which are SPI pins
#include <Wire.h> 

// external ICs
#include <LEDDriver.h>  // our gpio expander
#include <HDC1080_HumiditySensor.h> // humidity and temperature

// standard issue
#include <stdio.h>
#include <stdlib.h>

// initialize i2c devices
HumiditySensor t_h;
LEDDriver gpio;

/***************************
* for wireless communication
* change for each device
***************************/
device_t role = DEVICE0;

/*********************************
* DEVICE0: small_thresh = .4V   (500 and 3700)    | large_thresh = 3.0 V (3723 ADC)
* DEVICE1: small_thresh = .25V  (310 and 3700)   | large_thresh = 3.0 V (3723 ADC)
* DEVICE2: small_thresh = .25V  (310 and 3700)   | large_thresh = 3.0 V (3723 ADC)
***********************************/

// calibration constants for the sensor response
int small_thresh = 700; // in adc units  (496 = 0.4V) (310 = 0.25V)
int large_thresh = 3700; // in adc units


// constants for rfduino gpio
const int CS = 3; // SPI cs for ADC

// modify the rst of the rfduino spi things in variants.h  (/hardware/arduino/RFduino/variants/RFduino/variant.h)
const int wake = 6; // waking up from ULP

// constants for expander gpio 
const int laser = 0;
const int fan = 1;
const int s_serial = 2;
const int r = 3;
const int b = 4;
const int g = 5;
const int a_en = 6;



void setup() {
  
   /* set gzll parameters */
  RFduinoGZLL.begin(role);
  
  pinMode(CS, OUTPUT); // neccesary? 
//  SPI.begin();
  
  // configure wakeup pin as input
  pinMode(wake, INPUT_PULLUP); 
  
  // configure the gpio expander`
  Wire.beginOnPins(1,0); //(scl,sda) 
  gpio = LEDDriver();
  gpio.setup(5);
  gpio.setMaxIntensity(1,1);
  
  
  // init the sensor values
  data.small = 0;
  data.large = 0;
  data.humidity = 50.4;
  data.temperature = 9000.001;
  
  // blink white, then go to sleep
  
  gpio.setLEDBehavior(r, gpio.LEDonMax0);
  gpio.setLEDBehavior(g, gpio.LEDonMax0);
  gpio.setLEDBehavior(b, gpio.LEDonMax0);
  
  gpio.sendCommands(); 
  
  delay(500); 

  gpio_sleep();
  
  RFduino_pinWakeCallback(wake, LOW, onWake);
  RFduino_ULPDelay(INFINITE); // go to sleep forever, until the button goes low 
  
}

int onWake(uint32_t ulPin) { // wake up please
    
  delay(200); // simple debouncing
  
  // check for longclick
  unsigned long start_millis = millis();
  while (digitalRead(wake) == LOW) { }
  unsigned long end_millis = millis(); 
  
  
  if (end_millis - start_millis > 500) { // send data if long click   
    broadcast();
  } 
  else {  // sample if short click
    count();  
  }
  
  // gotta reset the pin 
  RFduino_resetPinWake(wake);
  return(0); // go back to sleep
}



void loop() {
  // nothing to see here
}


