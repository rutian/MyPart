

#include <SPI.h> 
const int CS = 2;
const int testPin = 6;
const int small_thresh = 1000;
const int large_thresh = 2000;


volatile int sc = 0; // small count
volatile int lc = 0; // large count

volatile unsigned int particle_check[] = {65535, 65535}; // used to check if particles pass a threshold 
volatile unsigned int i = 0;
 
void setup() {

  timer_1_config();
  
  Serial.begin(115200);  
  pinMode(CS, OUTPUT); // must manually toggle CS for nrf51*
  pinMode(testPin, OUTPUT);
  NRF_GPIO->OUTCLR = (1 << testPin); // set init state of testpin
//  SPI.begin();

}

/********************************************
* Timer configuration for 20us sampling period
*********************************************/
void timer_1_config(void) 
{
  NRF_TIMER1->TASKS_STOP = 1;   // Stop timer
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;  // fancy name for 0 (timer mode
  NRF_TIMER1->BITMODE = (TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos);
  NRF_TIMER1->PRESCALER = 4;   // SysClk/2^PRESCALER) =  16,000,000/16 = 1us resolution
  NRF_TIMER1->TASKS_CLEAR = 1; // Clear TIMER
  NRF_TIMER1->CC[0] = 20; // 20 us capture/compare register
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;  // set interrupt enable in  the compare[0] position
  NRF_TIMER1->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);

  NRF_TIMER1->TASKS_START = 1;   // Start TIMER
}

/***************************
* Our sampling interrupt
* 
****************************/
void TIMER1_IRQHandler(void)
{
  
  if (NRF_TIMER1->EVENTS_COMPARE[0] != 0) // for the 20us timer 
  {    
   
     NRF_GPIO->OUTCLR = (1 << CS);
     delay(5);
     NRF_GPIO->OUTSET = (1 << CS);
     
     
//   NRF_GPIO->OUTCLR = (1 << CS);
//  
//  //begin spi stuff
//  NRF_SPI0->TXD = 0x0;
//  NRF_SPI0->TXD = 0x0; // double buffer
//  
//  /******* first byte ***********/
//  while (NRF_SPI0->EVENTS_READY == 0)
//    ;
//  NRF_SPI0->EVENTS_READY = 0;
//  byte data1 = NRF_SPI0->RXD;  
//  
//  /******* second byte ***********/
//  while (NRF_SPI0->EVENTS_READY == 0)
//    ;
//  NRF_SPI0->EVENTS_READY = 0;
//  byte data2 = NRF_SPI0->RXD;  
//  
//  /******* release CS ***********/
//  NRF_GPIO->OUTSET = (1 << CS); // digitalWrite(CS, HIGH);
//  
//  /************ math begin **************************/
//
//  NRF_GPIO->OUTSET = (1 << testPin); // start timing
//  int data = data2 | (data1 << 8); // concatenate the two things
//  byte curr = i&1;
//  byte prev = (i-1)&1;
//  particle_check[curr] = data; // write data into our circular buffer
//  
//  if  ( particle_check[curr] > small_thresh && particle_check[prev] < small_thresh) {
//    sc++;  
//  }
//  
//  if  ( particle_check[curr] > large_thresh && particle_check[prev] < large_thresh) {
//    lc++;  
//  }
//  
//  i++;
//  

  NRF_TIMER1->EVENTS_COMPARE[0] = 0;
  }
}

void loop() {
  
  
  
}
