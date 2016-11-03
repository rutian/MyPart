#ifndef HDC1080_sensor_H
#define HDC1080_sensor_H
 
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