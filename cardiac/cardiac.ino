/* Audio Sample Player

   David Johnson-Davies - www.technoblogy.com - 29th September 2014
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

/* Direct-coupled capacitorless output */

#include <avr/pgmspace.h>
#include <avr/sleep.h>
#define adc_disable()  (ADCSRA &= ~(1<<ADEN))

// Audio encoded as unsigned 8-bit, 8kHz sampling rate
#include "heartbeat.h"

unsigned int p = 0;
unsigned int beat_delay = 0;
//volatile unsigned int led_state = LOW;

void setup() {
  // Enable 64 MHz PLL and use as source for Timer1
  PLLCSR = 1<<PCKE | 1<<PLLE;     
 
  // Set up Timer/Counter1 for PWM output
  TIMSK = 0;                              // Timer interrupts OFF
  TCCR1 = 1<<PWM1A | 2<<COM1A0 | 1<<CS10; // PWM A, clear on match, 1:1 prescale
  GTCCR = 1<<PWM1B | 2<<COM1B0;           // PWM B, clear on match
  OCR1A = 128; OCR1B = 128;               // 50% duty at start

  // Set up Timer/Counter0 for 8kHz interrupt to output samples.
  TCCR0A = 3<<WGM00;                      // Fast PWM
  TCCR0B = 1<<WGM02 | 2<<CS00;            // 1/8 prescale
  TIMSK = 1<<OCIE0A;                      // Enable compare match
  OCR0A = 124;                            // Divide by 1000

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  pinMode(4, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(0, OUTPUT); // LED
}

void loop() {}

// Sample interrupt
ISR(TIMER0_COMPA_vect) {

  // End of data? Go to sleep
  if (p >= heartbeat_wav_len) {
    digitalWrite(0, LOW);    
    beat_delay++;

    if (beat_delay > 6000) {   
      p=0; beat_delay=0;
    }
    
  }else{
    uint8_t sample = pgm_read_byte(&heartbeat_wav[p++]);
    if(900 < p && p < 2000){
        digitalWrite(0, LOW);
      }else{
        digitalWrite(0, HIGH);
      }  
    OCR1A = sample; OCR1B = sample ^ 255;
  }
}
