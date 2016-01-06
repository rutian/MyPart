/*****************************************************************
*
* Simple code to interface air particle counter with outside world
* 
* normally in ULP sleep
*
* when awoken, (1) shine lights while fan warms up, (2) dim lights,
* (3) sample for 5 seconds, (4) shine leds according to numPeaks,
* (5) broadcast that data for 10 seconds over bluetooth
*
* Rundong Tian 
* 9/13/2015
*
*****************************************************************/

#include <RFduinoBLE.h>
#include <SPI.h>  // need to modify board def to change which are SPI pins
#include <Wire.h>
#include "TLC59108.h" // 8 channel LED driver
#include "Adafruit_HDC1000.h" // humidity/temperature sensor
#include "PCA9536.h" // gpio expander

#include <stdio.h>
#include <stdlib.h>

Adafruit_HDC1000 hdc = Adafruit_HDC1000(); // init humidty/temp object
PCA9536 gpio;

//for the 8 leds
#define I2C_ADDR TLC59108::I2C_ADDR::SUB4
TLC59108 leds(I2C_ADDR);
const int numled = 8;


const int CS = 3; // SPI cs for ADC
const int wake = 6; // waking up from ULP

// for gpio
const int laser = 1;
const int fan = 2;
const int ledRST = 0;

// variables for particle counting
const int small_thresh = 370; // ~.3 Volts
const int large_thresh = 1500; // ~1.5 Volts
const int hyst = 10; // maybe for later

// keep track of counts
int small_count = 0;
int large_count = 0; 


// for BLE
const int interval = 675; // interval (ms) for BLE broadcast 500 - 1000 is sweet spot

/****************************************
* function to run upon wakeup 
* this function does most of the work. 
* blinks the lights, counts the particles, 
* measures humidity/temperature, yelss out over BLE 
*
****************************************/
int onWake(uint32_t ulPin) { // you've been frenched!!!
   
  RFduino_resetPinWake(wake);
  
  // turn on the fan and laser
  gpio.writePin(fan,1);
  gpio.writePin(laser,1);
  
  // briefly make some lights as the fan is spinning up
  leds.init();
  leds.setLedOutputMode(TLC59108::LED_MODE::PWM_IND);
  
  uint8_t gap = 130;
  int zero = 0;
  leds.setAllBrightness(0xff);
  delay(gap);
  leds.setAllBrightness(zero);
  delay(gap);
  leds.setAllBrightness(0xff);
  delay(gap);
    
  for(int pwm = 0x0ff; pwm >= 0x00; pwm--) {
      leds.setAllBrightness(pwm);
      delay(1);
  }

  fade2zero(8); 
     
  // read the humidity
  int b = hdc.begin();
  float temp = hdc.readTemperature();
  float hum  = hdc.readHumidity();
   
  // reset the number of counts
  
  small_count = 0;
  large_count = 0;
  int total_count = 0;
  int total_count_prev = 0; // helps us keep track of current led state

  
  // count for 15 seconds
  for (int i = 0; i < 15; i++) { 
    int data = count_one_second(); 
    int data_copy = data;
    
    small_count += ((data_copy << 16) >> 16);
    large_count += (data >> 16);
    
    //shine some lights
    total_count =  small_count; // small count is secretly all counts
    counts2lights(total_count_prev, total_count); // from, to, wait
    total_count_prev = total_count;// update prev
  }
    
  // turn off all gpios.
  gpio.writePin(fan,0);
  gpio.writePin(laser,0); 
  
  char small_str[6] ; 
  char large_str[6] ;
  
  
  
  sprintf(small_str,"%d",min(small_count - large_count, 999999));
  sprintf(large_str,"%d",min(large_count, 999999));
  
  strcat(small_str, ",");
  strcat(small_str, large_str);
  // put the two together
  
  // ble broadcast
  RFduinoBLE.advertisementData = small_str;
  
  // start the BLE stack
  RFduinoBLE.begin();
  // advertise for ms milliseconds
  RFduino_ULPDelay(10000);
  // stop the BLE stack
  RFduinoBLE.end();
   
  fade2zero(1);
  return(0); // go back to sleep
  
}



/****************************
* helper function for mapping the number of particles
* to light levels
*****************************/
void counts2lights(int from, int to) {
  
  int scale = 10; // scaling the input values to get full range on the lights
  
  // we have 2047 discrete levels we can represent
  from = min(from * scale, 2047);
  to   = min(to * scale, 2047);

  for (int i = from; i < to ; i+=32) {
    uint8_t channel = i/256; 
    uint8_t channel_brightness = i%256;
    leds.setBrightness(channel, channel_brightness);
  }
      
}

/****************************
* helper function for fadding all of the lights
* after they commmunicate the number of particles in the air
*****************************/
void fade2zero(int wait) {
  
  // read current light level
  uint8_t dutyCycles[8];
  leds.getAllBrightness(dutyCycles);
  
  // fade from the top
  for (int i = 7 ; i >= 0 ; i-- ) { // fade from top led
    for (int j = dutyCycles[i]; j>=0; j--) { // fade each led to zero from it's current value
        leds.setBrightness(i, j);
        delay(wait);
    } 
  }

}
 
void setup() {
  
  pinMode(CS, OUTPUT); // must manually toggle CS for nrf51*
  
  Wire.beginOnPins(1,0);
  gpio = PCA9536(); 
  gpio.configurePins(1, 0, 0, 0); // configure gpio as all output
  // turn everything off
  gpio.writePin(3, 0);
  gpio.writePin(fan, 0);
  gpio.writePin(laser, 0);

  
  /* setup the wakeup pin */
  pinMode(wake, INPUT_PULLUP); // configure wakeup pin as input
  RFduino_pinWakeCallback(wake, LOW, onWake); 
  
  /* set up a few ble parameters */ 
  RFduinoBLE.deviceName = "myPart";
  RFduinoBLE.advertisementInterval = 500; // 500 ms
  RFduinoBLE.txPowerLevel = 4; //-20 to +4 in 4 dB increments
  
  RFduino_ULPDelay(INFINITE); // go to sleep forever, until you've been frenched by a handsome prince

}

/*******************
*  Samples for 1 second, reports back big and small counts
*  returns int: large counts as top 16 bits, small counts as lower 16 bits 
*  current sample rate: 
********************/

int count_one_second(void) 
{

  unsigned int i = 0;
  unsigned int lc = 0; // large counts
  unsigned int sc = 0; // small counts
  unsigned int particle_check[] = {65535, 65535}; // tiny buffer to check if particles are big enough
   
   SPI.begin(); 
  
  // for now, determine the cutoff count emperically
  while (i < 87720) { // number of samples determined emperically. Sampling as fast as processing 'algorithm' will allow
    
    NRF_GPIO->OUTCLR = (1 << CS); // chip select
     
    //begin spi stuff
    NRF_SPI0->TXD = 0x0;
    NRF_SPI0->TXD = 0x0; // double buffer
    
    /******* first byte ***********/
    while (NRF_SPI0->EVENTS_READY == 0)
    ;
    NRF_SPI0->EVENTS_READY = 0;
    byte data1 = NRF_SPI0->RXD;  
    
    /******* second byte ***********/
    while (NRF_SPI0->EVENTS_READY == 0)
    ;
    NRF_SPI0->EVENTS_READY = 0;
    byte data2 = NRF_SPI0->RXD;  
    
    /******* release CS ***********/
    NRF_GPIO->OUTSET = (1 << CS); // digitalWrite(CS, HIGH);
    
    /************ math begin **************************/
    int data = data2 | (data1 << 8); // concatenate the two things
    byte curr = i&1;
    byte prev = (i-1)&1; // TODO: check for corner case at beginning or end of int
    
    particle_check[curr] = data; // write data into our "circular buffer"
    
    // check for sizing
    if  ( particle_check[curr] > small_thresh && particle_check[prev] < small_thresh) {
      sc++;  
    }
    if  ( particle_check[curr] > large_thresh && particle_check[prev] < large_thresh) {
      lc++;  
    }
    
    i++;
    
  } // end while
  
  SPI.end();
  return (lc << 16) | sc; // large counts (lc) as top 16 bits, sc as lower 16 bits 

  
}


/********************************************
* Timer configuration for 20us sampling period
*********************************************/
void timer_1_config(void) 
{
  NRF_TIMER1->TASKS_STOP = 1;   // Stop timer
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;  // fancy name for 0 (timer mode
  NRF_TIMER1->BITMODE = (TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos);
  NRF_TIMER1->PRESCALER = 4;   // SysClk/2^PRESCALER) =  16,000,000/16 = 1us resolution
  NRF_TIMER1->TASKS_CLEAR = 1; // Clear TIMER
  NRF_TIMER1->CC[0] = 20; // 20 us capture/compare register
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;  // set interrupt enable in  the compare[0] position
  NRF_TIMER1->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);

  NRF_TIMER1->TASKS_START = 1;   // Start TIMER
}

/***************************
* Timer interrupt, potentially used for sampling at a more consistent rate
* 
****************************/
void TIMER1_IRQHandler(void)
{
  
  if (NRF_TIMER1->EVENTS_COMPARE[0] != 0) // for the 20us timer 
  {    
   
  NRF_TIMER1->EVENTS_COMPARE[0] = 0; // clear the register
  }
}

void loop() { // nothing in loop
//  int data = count_one_second(); 
//  small_count += ((data << 16) >> 16);
//  large_count += (data >> 16);
//  Serial.print("small counts: ");
//  Serial.print(small_count);
//  Serial.print(" large counts: ");
//  Serial.println(large_count);
  
  
}
