// Author: Cordell Freeman, Hernani Fernandes, Philippe Cutillas, Sam Badger        
// Net ID: cordellfreeman, fernandes, pcutillas , smalbadger      
// Date: Mar-25-2019         
// Assignment: Final Project
//
// Description: This file contains a programmatic overall description of the
// program. It should never contain assignments to special function registers
// for the exception key one-line code such as checking the state of the pin.
//
#include "lcd.h"
#include "piezo.h"
#include "i2c.h"
#include "mq3.h"
#include "timer.h"
#include "switch.h"

#include <Arduino.h>

#define DEBOUNCE_DELAY 20

typedef enum states_t {wait_press, wait_release, debounce_press, debounce_release} states;
volatile states state = wait_press;
volatile bool on;

int main(void) {
  Serial.begin(9600);
  sei();
  initI2C();
  initSwitchPB3();
  initTimer0();

  on = false;

  while (1){
  switch(state){
      case(debounce_release):   //If in debounce release, delay to remove erroneous signals
        delayMs(DEBOUNCE_DELAY);
        state = wait_press;
        break;
      case(debounce_press):     //If in debounce press, delay to remove erroneous signals
        delayMs(DEBOUNCE_DELAY);
        state = wait_release;
        break;
      default:
        break;
    }
    /*if (on){
      turnDisplayOn();
      displayValue(0x06);
    }
    else{
      turnDisplayOff();
    }*/
  }
}

/* Implement an Pin Change Interrupt which handles the switch being
* pressed and released. When the switch is pressed and released, the motors turn on and off.
*/
ISR(PCINT0_vect){
  if(state == wait_press) { //Move to debounce press which will delay and remove err signals
    on = !on;
    if (on){
      turnDisplayOn();
      displayValue(.08);
    }
    else{
      turnDisplayOff();
    }
    state = debounce_press;
  }
  if(state == wait_release) { //If in wait release state, toggle on_off
    state = debounce_release; //Move to bounce release to transition back to waiting for a button press
  }
}