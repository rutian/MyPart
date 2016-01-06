// adc sampling, and do math for peak detection


#include <SPI.h> 



const int CS = 2;
const int testPin = 6;
const int small_thresh = 1000;
const int large_thresh = 2000;
int sc = 0; // small count
int lc = 0; // large count
unsigned int particle_check[] = {65535, 65535}; // used to check if particles pass a threshold 
unsigned int i = 0;

void setup() {

  // default configuratino of SPI bus is 4MHz
  
  Serial.begin(9600);  
  pinMode(CS, OUTPUT); // must manually toggle CS for nrf51*
  pinMode(testPin, OUTPUT);
  NRF_GPIO->OUTCLR = (1 << testPin); // set init state of testpin
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
  
  /******* first byte ***********/
  while (NRF_SPI0->EVENTS_READY == 0)
    ;
  NRF_SPI0->EVENTS_READY = 0;
  data1 = NRF_SPI0->RXD;  
  
  /******* second byte ***********/
  while (NRF_SPI0->EVENTS_READY == 0)
    ;
  NRF_SPI0->EVENTS_READY = 0;
  data2 = NRF_SPI0->RXD;  
  
  /******* release CS ***********/
  NRF_GPIO->OUTSET = (1 << CS); // digitalWrite(CS, HIGH);
  
  

  /************ math begin **************************/

  NRF_GPIO->OUTSET = (1 << testPin); // start timing
  data = data2 | (data1 << 8); // concatenate the two things
  int curr = i&1;
  int prev = (i-1)&1;
  particle_check[curr] = data; // write data into our circular buffer
  
  if  ( particle_check[curr] > small_thresh && particle_check[prev] < small_thresh) {
    sc++;  
  }
  
  if  ( particle_check[curr] > large_thresh && particle_check[prev] < large_thresh) {
    lc++;  
  }
  
  i++;
  
  // for timing
  NRF_GPIO->OUTCLR = (1 << testPin); 
  

//    
//  Serial.print("small count: ");
//  Serial.println(sc);
//  Serial.print("large count: ");
//  Serial.println(lc);
//
//  
//
//  
//  delay(500);


}
