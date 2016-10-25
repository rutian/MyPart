#ifndef TCA6507_driver_H
#define TCA6507_driver_H
 
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
        void setALDIntensity(int percent);

        void setLEDBehavior(int led, int behavior);

        void sleep();
        void wake();

        enum { 
            LEDoff = 0,
            PinOn = 1,
            LEDblink0 = 2,
            LEDblink1 = 3,
            LEDonMaster = 4,
            LEDonMax0 = 5,
            LEDonMax1 = 6,
        };
};
 
#endif