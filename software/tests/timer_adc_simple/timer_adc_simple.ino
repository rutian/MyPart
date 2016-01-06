

// triggering adc samples with timer interrupts


#include <SPI.h>


const int testPin = 6;
const int CS = 2;

const int small_thresh = 1000;
const int large_thresh = 2000;

/******* variables accessed in ISR *************/
volatile int sc = 0; // small count
volatile int lc = 0; // large count

volatile unsigned int particle_check[] = {65535, 65535}; // used to check if particles pass a threshold 
volatile unsigned int i = 0;



 
void setup() {

    timer_1_config(); // configure some shit 
    
    pinMode(testPin, OUTPUT);
    pinMode(CS, OUTPUT); // must manually toggle CS for nrf51*
    SPI.begin();
    

}


void timer_1_config(void)
{
  NRF_TIMER1->TASKS_STOP = 1;   // Stop timer
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;  // (TIMER_MODE_MODE_Timer = 1) => for timer mode
  NRF_TIMER1->BITMODE = (TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos);
  NRF_TIMER1->PRESCALER = 4;   // SysClk/2^PRESCALER) =  16,000,000/16 = 1us resolution
  NRF_TIMER1->TASKS_CLEAR = 1; // Clear timer
  NRF_TIMER1->CC[0] = 26; // 20 us
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos; // set up an interrupt for CC[0]
  NRF_TIMER1->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
  attachInterrupt(TIMER1_IRQn, TIMER1_IRQHandler);    // also used in variant.cpp to configure the RTC1
  NRF_TIMER1->TASKS_START = 1;   // Start TIMER
}


void TIMER1_IRQHandler(void)
{
  if (NRF_TIMER1->EVENTS_COMPARE[0] != 0)
  {
    
    NRF_GPIO->OUTCLR = (1 << CS); // activate CS
   
    //begin spi stuff
    NRF_SPI0->TXD = 0x0;
    NRF_SPI0->TXD = 0x0; // double buffer
    
    /******* first byte ***********/
    while (NRF_SPI0->EVENTS_READY == 0)
    ;
    NRF_SPI0->EVENTS_READY = 0;
    byte data1 = NRF_SPI0->RXD;  
    /******* second byte ***********/
    while (NRF_SPI0->EVENTS_READY == 0)
    ;
    NRF_SPI0->EVENTS_READY = 0;
    byte data2 = NRF_SPI0->RXD;  
    /********** release cs **********/
    NRF_GPIO->OUTSET = (1 << CS); 
    
    unsigned short data = data2 | (data1 << 8); // concatenate the two things
    
    /************ math begin **************************/
    
    int curr = i&1;
    int prev = (i-1)&1;
    particle_check[curr] = data;
    
    if  ( particle_check[curr] > small_thresh && particle_check[prev] < small_thresh) {
      sc++;  
    }
  
    if  ( particle_check[curr] > large_thresh && particle_check[prev] < large_thresh) {
    lc++;  
    }
  
    i++; // increment our index
    
    NRF_TIMER1->EVENTS_COMPARE[0] = 0; // clear the interrupt (use shorts to not have to do this? 
    
  }
}

void loop() {
  // nothing to see here
}
