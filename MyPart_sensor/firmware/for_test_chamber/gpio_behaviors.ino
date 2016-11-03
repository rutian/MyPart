/**************************************************************
* put the gpios to their appropriate states so that the sensor 
* can take a sample
***************************************************************/
void gpio_device_sampling() {  
  
  Wire.beginOnPins(1,0); //(scl,sda) 
  
  // turn off the leds
  gpio.setLEDBehavior(r, gpio.LEDoff); 
  gpio.setLEDBehavior(g, gpio.LEDoff);
  gpio.setLEDBehavior(b, gpio.LEDoff);
  
  // turn off the open drain, and the pullup will turn on these peripherals
  gpio.setLEDBehavior(fan, gpio.LEDoff); 
  gpio.setLEDBehavior(laser, gpio.LEDoff);
  gpio.setLEDBehavior(a_en, gpio.LEDoff); 
  
  gpio.sendCommands(); 
  
}

/*******************************************
* return all gpio pins to the sleep state
*******************************************/
void gpio_sleep() {
  
   Wire.beginOnPins(1,0); //(scl,sda) 
   
   gpio.setLEDBehavior(r, gpio.LEDoff);
   gpio.setLEDBehavior(g, gpio.LEDoff);
   gpio.setLEDBehavior(b, gpio.LEDoff);
   
   gpio.setLEDBehavior(fan, gpio.PinOn); // on means off (output is open drain, hooked up to pullup [in v0 of the board at least])
   gpio.setLEDBehavior(laser, gpio.PinOn); 
   gpio.setLEDBehavior(a_en, gpio.PinOn); 
 
   gpio.sendCommands(); 
 
}


/********************
test power consumption
if things weren't open drain
*********************/
void gpio_no_drain(){
   Wire.beginOnPins(1,0); //(scl,sda) 
   gpio.setLEDBehavior(r, gpio.LEDoff);
   gpio.setLEDBehavior(g, gpio.LEDoff);
   gpio.setLEDBehavior(b, gpio.LEDoff);
   gpio.setLEDBehavior(fan, gpio.LEDoff);
   gpio.setLEDBehavior(laser, gpio.LEDoff);
   gpio.setLEDBehavior(a_en, gpio.LEDoff);
   
   gpio.sendCommands(); 

   
  
}
