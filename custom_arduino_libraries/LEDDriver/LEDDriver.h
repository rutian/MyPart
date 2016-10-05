#ifndef LEDDriver_H
#define LEDDriver_H
 
#include <Arduino.h>
#include <Wire.h>
 
class LEDDriver {
  public:
        LEDDriver();
        ~LEDDriver();
        void setup(int en_pin);
        void sendCommands();

        void allOff();

        void setFadeOnTime(int ms_1, int ms_2);
        void setFullyOnTime(int ms_1, int ms_2);
        void setFadeOffTime(int ms_1, int ms_2);
        void setFullyOffTime1(int ms_1, int ms_2);
        void setFullyOffTime2(int ms_1, int ms_2);
        void setMaxIntensity(int percent1, int percent2);

        void setLEDBehavior(int led, int behavior);

        void sleep();
        void wake();

        enum { 
            LEDoff = 0,
            LEDon = 1,
            LEDblink = 2,
        };
};
 
#endif