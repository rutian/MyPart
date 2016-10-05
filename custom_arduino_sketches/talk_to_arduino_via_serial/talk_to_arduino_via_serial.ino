
//reference: http://bildr.org/2011/01/arduino-serial/

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

Servo sv;

int active_pin = 4;
int servo_pin = 9;
int off_degree = 90;
int on_degree = 40;

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);


//these signals need to match the python code
char PIN_ON = 'a';
char PIN_OFF = 'b';
char SERVO_ON = 'c';
char TAKE_AMBIENT_READS = 'd';

void configureSensor() {
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
}

void setup() {
  Serial.begin(9600);
  //Set all the pins we need to output pins
  pinMode(active_pin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  sv.attach(servo_pin, 771, 1798);
  if(!tsl.begin()) {
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  configureSensor();
}

void loop() {
  if (Serial.available()) {
    char ser = Serial.read();
    if (ser  == PIN_ON) {
      digitalWrite(active_pin, HIGH);
      digitalWrite(LED_BUILTIN, HIGH);
   } else if (ser == PIN_OFF) {
      digitalWrite(active_pin, LOW);
      digitalWrite(LED_BUILTIN, LOW); 
   } else if (ser == SERVO_ON) {
      sv.write(on_degree);
      delay(500);
      sv.write(off_degree);
   } else if (ser == TAKE_AMBIENT_READS) {
      sensors_event_t event;
      tsl.getEvent(&event);
      byte *b = (byte*) &event.light;
      Serial.write(b, 4);
   }
  }
}
