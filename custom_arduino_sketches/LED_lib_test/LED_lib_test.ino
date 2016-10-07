#include <Wire.h>
#include <LEDDriver.h>
#include <HumiditySensor.h>
 
LEDDriver driver;
HumiditySensor sensor;

//mypart rfduino must use 5 and 6 for clock and sda

void setup()
{
  driver.setup(5);
//  sensor.setup();
}

void loop() {
  driver_test();
//  sensor_test();
  
  delay(2000);
}

void sensor_test() {
  sensor.getTemperature();
  sensor.getHumidity();
  
}

void driver_test() {
  driver.allOff();
 
  driver.setFullyOffTime1(500, 1500);
  driver.setFullyOffTime2(200, 1500);
  driver.setFadeOnTime(2000, 1000);
  driver.setMaxIntensity(5, 50);
  driver.setALDIntensity(100);

  driver.setLEDBehavior(0, driver.LEDonMaster);
  driver.setLEDBehavior(1, driver.LEDonMax0);
  driver.setLEDBehavior(2, driver.LEDonMax1);
//  driver.setLEDBehavior(3, driver.LEDblink);
//  driver.setLEDBehavior(4, driver.LEDblink);
//  driver.setLEDBehavior(6, driver.LEDblink);
  driver.sendCommands(); 
}
