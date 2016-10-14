
//reference: http://bildr.org/2011/01/arduino-serial/

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <HumiditySensor.h>

Servo sv;

int active_pin = 4;
int servo_pin = 3;
int mypart_pin = 6;
int off_degree = 90;
int on_degree = 40;

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
HumiditySensor humSensor = HumiditySensor();

//these signals need to match the python code
const char PIN_ON = 'a';
const char PIN_OFF = 'b';
const char SERVO_ON = 'c';
const char TAKE_AMBIENT_READS = 'd';
const char SAMPLE_MYPART = 'e';
const char SEND_MYPART = 'f';

void configureLightSensor() {
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
}

void setup() {
  Serial.begin(9600);
  //Set all the pins we need to output pins
  pinMode(active_pin, OUTPUT);
  pinMode(mypart_pin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  sv.attach(servo_pin, 771, 1798);
  if(!tsl.begin()) {
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  configureLightSensor();
  humSensor.setup();
}

void loop() {
  if (Serial.available()) {
    char ser = Serial.read();
    switch(ser) {
      case PIN_ON:
        digitalWrite(active_pin, HIGH);
        digitalWrite(LED_BUILTIN, HIGH);
        break;
      case PIN_OFF:
        digitalWrite(active_pin, LOW);
        digitalWrite(LED_BUILTIN, LOW); 
        break;
      case SERVO_ON:
        sv.write(on_degree);
        delay(500);
        sv.write(off_degree);
        break;
      case TAKE_AMBIENT_READS: {
        sensors_event_t event;
        tsl.getEvent(&event);
        byte *b1 = (byte*) &event.light;
        Serial.write(b1, 4);
        float temp = humSensor.getTemperature();
        float hum = humSensor.getHumidity();
        Serial.write((byte*) &temp, 4);
        Serial.write((byte*) &hum, 4);
        break;
      }
      case SAMPLE_MYPART: 
        digitalWrite(mypart_pin, HIGH);
        delay(400);
        digitalWrite(mypart_pin, LOW);
        break;
      case SEND_MYPART:
        digitalWrite(mypart_pin, HIGH);
        delay(1000);
        digitalWrite(mypart_pin, LOW);
        break;
    }
  }
}
