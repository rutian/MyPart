// tester function to just read the ADC
uint16_t spi_analogRead() {
  
  byte data1;
  byte data2;
  uint16_t data;
   

   
  SPI.begin();
  
  //  NRF_GPIO->OUTCLR = (1 << CS); // digitalWrite(CS,LOW)
  digitalWrite(CS,LOW);
  
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
  
  
//  NRF_GPIO->OUTSET = (1 << CS); // digitalWrite(CS, HIGH);
  digitalWrite(CS,HIGH);
  
  data = data2 | (data1 << 8); // concatenate the two things
  
  SPI.end();
  
  return data;
}
/*******************
*  Samples for 1 second, reports back big and small counts
*  returns int: large counts as top 16 bits, small counts as lower 16 bits 
*  current sample rate: 
********************/

int count_one_second(void) 
{
  unsigned int i = 0;
  unsigned int lc = 0; // large counts
  unsigned int sc = 0; // small counts
  unsigned int particle_check[] = {65535, 65535}; // tiny buffer to check if particles are big enough
   
   SPI.begin(); 
  
  // for now, determine the cutoff count emperically
  while (i < 87720) { // number of samples determined emperically. Sampling as fast as processing 'algorithm' will allow
    
    NRF_GPIO->OUTCLR = (1 << CS); // chip select
     
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
    
    /******* release CS ***********/
    NRF_GPIO->OUTSET = (1 << CS); // digitalWrite(CS, HIGH);
    
    /************ math begin **************************/
    int data = data2 | (data1 << 8); // concatenate the two things
    byte curr = i&1;
    byte prev = (i-1)&1; // TODO: check for corner case at beginning or end of int
    
    particle_check[curr] = data; // write data into our "circular buffer"
    
    // check for sizing
    if  ( particle_check[curr] > small_thresh && particle_check[prev] < small_thresh) {
      sc++;  
    }
    if  ( particle_check[curr] > large_thresh && particle_check[prev] < large_thresh) {
      lc++;  
    }
    
    i++;
    
  } // end while
  
  SPI.end();
  return (lc << 16) | sc; // large counts (lc) as top 16 bits, sc as lower 16 bits 

  
}
