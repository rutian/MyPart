
//register calls for back to back (double buffered) spi read


#include <SPI.h> 

const int CS = 2;

void setup() {

  // default configuratino of SPI bus is 4MHz
  
  Serial.begin(9600);  
  pinMode(CS, OUTPUT); // must manually toggle CS for nrf51*
  
  SPI.begin();
    
}

void loop() {
  
  byte data1;
  byte data2;
  unsigned short data;
   
  NRF_GPIO->OUTCLR = (1 << CS);
  

 
  //begin spi stuff
  NRF_SPI0->TXD = 0x0;
  NRF_SPI0->TXD = 0x0; // double buffer
  
  // first byte
  while (NRF_SPI0->EVENTS_READY == 0)
    ;
  NRF_SPI0->EVENTS_READY = 0;
  data1 = NRF_SPI0->RXD;  
  
  // second byte
  while (NRF_SPI0->EVENTS_READY == 0)
    ;
  NRF_SPI0->EVENTS_READY = 0;
  data2 = NRF_SPI0->RXD;  
  
  
 NRF_GPIO->OUTSET = (1 << CS); // digitalWrite(CS, HIGH);
  
  data = data2 | (data1 << 8); // concatenate the two things
  
  Serial.print("all bits: ");
  Serial.println(data);



}
