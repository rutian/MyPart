
//reference: http://bildr.org/2011/01/arduino-serial/

#include <Servo.h>

Servo sv;

int active_pin = 4;
int servo_pin = 9;
int off_degree = 90;
int on_degree = 40;

//these signals need to match the python code
char PIN_ON = 'a';
char PIN_OFF = 'b';
char SERVO_ON = 'c';

void setup() {
  Serial.begin(9600);
  //Set all the pins we need to output pins
  pinMode(active_pin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  sv.attach(servo_pin, 771, 1798);
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
   }
  }
}
