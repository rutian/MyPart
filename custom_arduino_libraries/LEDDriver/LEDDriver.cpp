#include "LEDDriver.h"

// ----- Globals -----

const byte led_driver_address = B1000101;

const byte default_time = B01000100;
const byte default_max_intensity = B11111111;
const byte default_master_intensity = B00001111;
const byte default_ald_intensity = B00000000;

const byte command = B00010000;

//code is index; ms is value
const int time_params[16] = {0, 64, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 5760, 8128, 16320};
const float percent_params[16] = {0, 6.66, 13.32, 19.98, 26.64, 33.33, 39.96, 46.62, 53.28, 59.94, 66.66, 73.26, 79.92, 86.58, 93.24, 100};

int en_pin;

//first half of the byte defines bank0 behavior
//second half of the byte defines bank1 behavior
byte fade_on_time = default_time;
byte full_on_time = default_time; 
byte fade_off_time = default_time;
byte full_off_time_1 = default_time;
byte full_off_time_2 = default_time;  
byte max_intensity = default_max_intensity;

//bits 0-3 set ALD value (0-15)
//bits 4-5 determine whether the maximum intensity of PWM0 and PWM1 is set by the programmed F value (BRIGHT_F0 or BRIGHT_F1) or the master ALD value
//The default value for these bits is 0
// Bits 6–7 determine whether each PWM operates in normal or one-shot mode
//see page 25 of datasheet
byte ald_intensity = default_ald_intensity;

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
  Wire.write(ald_intensity);
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

void LEDDriver::setALDIntensity(int percent) {
  byte b1 = 0; //if want control over one-shot mode, etc., modify this
  byte b0 = percentToScale(percent);
  ald_intensity = (b1 << 4) | b0;
};


// --------- Setting Select Registers -----------
// Note: select_num counts from R to L

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

//100 LED fully on -- can be used as General Purpose Out
byte setLEDToOn(byte select, int led, int select_num) {
  if (select_num == 2) {
   return maskToOne(select, led);
  } else {
   return  maskToZero(select, led); 
  }
}
  
//110 blinking with bank 0 rules
byte setLEDToBlink_Bank0(byte select, int led, int select_num) {
  if (select_num == 0) {
    return maskToZero(select, led);
  } else {
   return maskToOne(select, led); 
  }
}

//111 blinking with bank 1 rules
byte setLEDToBlink_Bank1(byte select, int led, int select_num) {
   return maskToOne(select, led); 
}

//101 LED on at brightness set by Master Intensity 
byte setLEDToOnMasterIntensity(byte select, int led, int select_num) {
  if (select_num == 1) {
    return maskToZero(select, led);
  } else {
   return maskToOne(select, led); 
  }
}

//010  LED on with maximum intensity value of PWM0
byte setLEDToOnMaxIntensity_Bank0(byte select, int led, int select_num) {
  if (select_num == 1) {
    return maskToOne(select, led);
  } else {
   return maskToZero(select, led); 
  }
}

//011  LED on with maximum intensity value of PWM1
byte setLEDToOnMaxIntensity_Bank1(byte select, int led, int select_num) {
  if (select_num == 2) {
    return maskToZero(select, led);
  } else {
   return maskToOne(select, led); 
  }
}




// -------- LED Behaviors ----------

void LEDDriver::setLEDBehavior(int led, int behavior){
  for (int select_num = 0; select_num < 3; select_num++) {
    switch(behavior) {
      case LEDoff:
        selects[select_num] = setLEDToOff(selects[select_num], led, select_num); 
        break;
      case PinOn:
        selects[select_num] = setLEDToOn(selects[select_num], led, select_num); 
        break;
      case LEDblink0:
        selects[select_num] = setLEDToBlink_Bank0(selects[select_num], led, select_num); 
        break;
      case LEDblink1:
        selects[select_num] = setLEDToBlink_Bank1(selects[select_num], led, select_num); 
        break;
      case LEDonMaster:
        selects[select_num] = setLEDToOnMasterIntensity(selects[select_num], led, select_num); 
        break;
      case LEDonMax0:
        selects[select_num] = setLEDToOnMaxIntensity_Bank0(selects[select_num], led, select_num); 
        break;
      case LEDonMax1:
        selects[select_num] = setLEDToOnMaxIntensity_Bank1(selects[select_num], led, select_num); 
        break;
    }
  }
}

void LEDDriver::allOff() {
  for (int select_num = 0; select_num < 3; select_num++) {
    selects[select_num] = 0;
  }
}

