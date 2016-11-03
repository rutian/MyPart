
/********************************
* helper functions for what to do after short button press
********************************/
void count(){
   // green led for sampling
    // blink green to indicate sampling 
    for (int i = 0; i < 3; i++){
      gpio.setLEDBehavior(g, gpio.LEDonMax0);
      gpio.sendCommands(); 
      delay(200);
      gpio.setLEDBehavior(g, gpio.LEDoff);
      gpio.sendCommands(); 
      delay(200);
    } 
 
    // turn on the right peripherals
    gpio_device_sampling();
   

    delay(2000);

    
    int small_count = 0;
    int large_count = 0;

    // count for 45 seconds
    for (int i = 0; i < 45; i++) { 
      // every second
      
      int data = count_one_second(); 
      int data_copy = data;
      
      small_count += ((data_copy << 16) >> 16); // remove top 16 bits
      large_count += (data >> 16);              // remove bot 16 bits
     
      //todo:  helper function for updating the lightshow
 
    }

    
    data.small = small_count;
    data.large = large_count;
    
    // start the i2c and take some readings    
    t_h = HumiditySensor(); 
    data.temperature = t_h.getTemperature();
    data.humidity = t_h.getHumidity();


    gpio_sleep(); 
    
}

/******************************
* On long click, broadcast 
* the last pieces of data
******************************/
void broadcast(){
     
    // blink blue to indicate broadcast 
    Wire.beginOnPins(1,0); //(scl,sda) 
    
    for (int i = 0; i < 3; i++){
      gpio.setLEDBehavior(b, gpio.LEDonMax0);
      gpio.sendCommands(); 
      delay(200);
      gpio.setLEDBehavior(b, gpio.LEDoff);
      gpio.sendCommands(); 
      delay(200);
    } 
    
    // setup gzll stack
   
    RFduinoGZLL.sendToHost((const char *)&data, sizeof(data));
    delay(2000);
    
    // turn all gpio to their default (sleeping conditions) 
    gpio_sleep();
    
}

