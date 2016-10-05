#include "LEDDriver.h"

// ----- Globals -----

const byte led_driver_address = B1000101;

const byte default_time = B01000100;
const byte default_max_intensity = B11111111;
const byte default_master_intensity = B00001111;

const byte command = B00010000;

//code is index; ms is value
const int time_params[16] = {0, 64, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 5760, 8128, 16320};
const float percent_params[16] = {0, 6.66, 13.32, 19.98, 26.64, 33.33, 39.96, 46.62, 53.28, 59.94, 66.66, 73.26, 79.92, 86.58, 93.24, 100};

int en_pin;

byte fade_on_time = default_time;
byte full_on_time = default_time; 
byte fade_off_time = default_time;
byte full_off_time_1 = default_time;
byte full_off_time_2 = default_time;  
byte max_intensity = default_max_intensity;

byte selects[3] = {0, 0, 0};

//------- Functions -------
  
//<<constructor>>
LEDDriver::LEDDriver(){}
 
//<<destructor>>
LEDDriver::~LEDDriver(){}

//call this in the setup function
void LEDDriver::setup(int ep) {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial);
  Serial.println("\n LED Driver Initiated");
  en_pin = ep;
  pinMode(en_pin, OUTPUT);
}

//write all commands to board
void LEDDriver::sendCommands(){
  Wire.beginTransmission(led_driver_address);
    Serial.print("\n Sending command to driver at ");
    Serial.print(led_driver_address, HEX);
    Serial.print("\n");
  Wire.write(command);
  //order of writes matters! 
  //will write to registers in order for each feature
  Wire.write(selects[0]);
  Wire.write(selects[1]);
  Wire.write(selects[2]);
  Wire.write(fade_on_time);
  Wire.write(full_on_time);
  Wire.write(fade_off_time);
  Wire.write(full_off_time_1);
  Wire.write(full_off_time_2);
  Wire.write(max_intensity);
  byte result = Wire.endTransmission();
  if (result == 0) {
    Serial.println("\n Successful command");
  } else {
    Serial.println("\n Error in sending command. Check device connection and address");
  }
}

void LEDDriver::sleep(){
  digitalWrite(en_pin, LOW); 
}

void LEDDriver::wake(){
  digitalWrite(en_pin, HIGH);
}
 

//------ Characteristics --------

//convert milliseconds to byte
//scale -- 0 to 16320 ms (exponential trend) to 0-15
byte msToScale(int ms) {
  //round up to the nearest increment
  for (int i = 0; i < 16; i++) {
    if (time_params[i] >= ms ){
      return i;
    }
  }
  return 15;
};

byte percentToScale(int percent) {
  //round up to the nearest increment
  for (int i = 0; i < 16; i++) {
    if (percent_params[i] >= percent ){
      return i;
    }
  }
  return 15;
};

void LEDDriver::setFadeOnTime(int ms_1, int ms_2) {
  byte b1 = msToScale(ms_2);
  byte b0 = msToScale(ms_1);
  fade_on_time = (b1 << 4) | b0;
}

void LEDDriver::setFullyOnTime(int ms_1, int ms_2){
  byte b1 = msToScale(ms_2);
  byte b0 = msToScale(ms_1);
  full_on_time = (b1 << 4) | b0;
};
void LEDDriver::setFadeOffTime(int ms_1, int ms_2){
  byte b1 = msToScale(ms_2);
  byte b0 = msToScale(ms_1);
  fade_off_time = (b1 << 4) | b0;
};
void LEDDriver::setFullyOffTime1(int ms_1, int ms_2){
  byte b1 = msToScale(ms_2);
  byte b0 = msToScale(ms_1);
  full_off_time_1 = (b1 << 4) | b0;
};
void LEDDriver::setFullyOffTime2(int ms_1, int ms_2){
  byte b1 = msToScale(ms_2);
  byte b0 = msToScale(ms_1);
  full_off_time_2 = (b1 << 4) | b0;
};
void LEDDriver::setMaxIntensity(int percent1, int percent2){
  byte b1 = percentToScale(percent2);
  byte b0 = percentToScale(percent1);
  max_intensity = (b1 << 4) | b0;
};


// --------- Setting Select Registers -----------

byte maskToZero(byte b, int place) {
  byte mask = 1 << place;
  mask = ~mask;
  return b & mask;
}

byte maskToOne(byte b, int place) {
  byte mask = 1 << place;
  return b | mask;
}

//000 LED off
//001 LED off
byte setLEDToOff(byte select, int led, int select_num) {
  return maskToZero(select, led);
}

//100 LED fully on
byte setLEDToOn(byte select, int led, int select_num) {
  if (select_num == 2) {
   return maskToOne(select, led);
  } else {
   return  maskToZero(select, led); 
  }
}
  
//110 blinking with bank 0 rules
byte setLEDToBlink(byte select, int led, int select_num) {
  if (select_num == 0) {
    return maskToZero(select, led);
  } else {
   return maskToOne(select, led); 
  }
}


// -------- LED Behaviors ----------
void LEDDriver::setLEDBehavior(int led, int behavior){
  for (int select_num = 0; select_num < 3; select_num++) {
     if (behavior == LEDoff) {
       selects[select_num] = setLEDToOff(selects[select_num], led, select_num); 
     } else if (behavior == LEDon) {
       selects[select_num] = setLEDToOn(selects[select_num], led, select_num); 
     } else if (behavior == LEDblink) {
       selects[select_num] = setLEDToBlink(selects[select_num], led, select_num); 
     }
  }
}

void LEDDriver::allOff() {
  for (int select_num = 0; select_num < 3; select_num++) {
    selects[select_num] = 0;
  }
}

