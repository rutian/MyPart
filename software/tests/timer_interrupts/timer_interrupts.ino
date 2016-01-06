

const int testPin = 6;

 
void setup() {

    timer_config();
    pinMode(testPin, OUTPUT);
      

}


void timer_config(void)
{
  NRF_TIMER1->TASKS_STOP = 1;   // Stop timer
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;  // (TIMER_MODE_MODE_Timer = 1) => for timer mode
  NRF_TIMER1->BITMODE = (TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos);
  NRF_TIMER1->PRESCALER = 4;   // SysClk/2^PRESCALER) =  16,000,000/16 = 1us resolution
  NRF_TIMER1->TASKS_CLEAR = 1; // Clear timer
  NRF_TIMER1->CC[0] = 20; // 20 us
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos; // set up an interrupt for CC[0]
  NRF_TIMER1->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
  attachInterrupt(TIMER1_IRQn, TIMER1_Interrupt);    // also used in variant.cpp to configure the RTC1
  NRF_TIMER1->TASKS_START = 1;   // Start TIMER
}


void TIMER1_Interrupt(void)
{
  if (NRF_TIMER1->EVENTS_COMPARE[0] != 0)
  {
    
    NRF_GPIO->OUTSET = (1 << testPin); // toggle some pins
    delayMicroseconds(5);
    NRF_GPIO->OUTCLR = (1 << testPin);

    NRF_TIMER1->EVENTS_COMPARE[0] = 0; // clear the event
  }
}

void loop() {
  // nothing to see here
}
