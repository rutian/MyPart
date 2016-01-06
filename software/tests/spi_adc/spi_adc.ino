
#include <my_rfduino_SPI.h>

// works, but not gapless transfer between the two bytes, and links to other library

const int CS = 2;

void setup() {

  // default configuratino of SPI bus is 4MHz
  
  Serial.begin(9600);  
  pinMode(CS, OUTPUT);
}

void loop() {
  
  byte data1 = 0xde;  
  byte data2 = 0xad;
   
  
  SPI.begin();
  digitalWrite(CS, LOW);
  byte return1 = SPI.transfer(data1);
  byte return2 = SPI.transfer(data2);
  
  digitalWrite(CS, HIGH);
  Serial.println(return1);
  Serial.println(return2);
  
  delay(500);


}
