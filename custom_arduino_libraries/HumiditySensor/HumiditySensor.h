#ifndef HumiditySensor_H
#define HumiditySensor_H
 
#include <Arduino.h>
#include <Wire.h>
 
class HumiditySensor {
  public:
        HumiditySensor();
        ~HumiditySensor();
        void setup();
        float getTemperature();
        float getHumidity();
};
 
#endif