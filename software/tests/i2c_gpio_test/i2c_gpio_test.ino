
#include <Wire.h>
#include "PCA9536.h"

PCA9536 gpio;

void setup() {
  Wire.beginOnPins(1,0);

  gpio = PCA9536();
  gpio.configurePins(0, 0, 0, 0); // configure all as output

 

}

void loop() {
  // put your main code here, to run repeatedly:
  gpio.writePin(3, 1);
  gpio.writePin(2, 1);
  gpio.writePin(1, 1);
  gpio.writePin(0, 1);

  delay(50);

  gpio.writePin(3, 0);
  gpio.writePin(2, 0);
  gpio.writePin(1, 0);
  gpio.writePin(0, 0);

  delay(50);


}
