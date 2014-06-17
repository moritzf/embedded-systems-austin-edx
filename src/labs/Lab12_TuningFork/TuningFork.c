// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3.
// There is an output on PA2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
// March 8, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#include "TExaS.h"
#include "..//tm4c123gh6pm.h"

#define SPEAKER GPIO_PORTA_DATA_R & 0x04
#define SWITCH GPIO_PORTA_DATA_R & 0x08

// Global variables

unsigned long reload_value = 90908; // 80 Mhz / 440 Hz - 1
unsigned long lastStatus = 0; // 1: pressed, 0: not pressed 
unsigned long nextAction = 0; // 1: loud, 0: quiet

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

// input from PA3, output to PA2, SysTick interrupts
void Sound_Init(void){ 
	SYSCTL_RCGC2_R |= 0x01;
	GPIO_PORTA_AMSEL_R &= ~0x0C;
  GPIO_PORTA_PCTL_R &= ~0x0000FF00;
  GPIO_PORTA_DIR_R |= 0x04;
  GPIO_PORTA_AFSEL_R &= ~0x0C;
  GPIO_PORTA_DEN_R |= 0x0C;
	GPIO_PORTA_PDR_R = 0x08;
  NVIC_ST_CTRL_R = 0;
  NVIC_ST_RELOAD_R = reload_value;
  NVIC_ST_CURRENT_R = 0; // clear current 
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;
  NVIC_ST_CTRL_R = 0x00000007;
	NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}

// called at 880 Hz
void SysTick_Handler(void){
	if(SWITCH) {
		if(!lastStatus) {
			nextAction ^= 1;
		}
	}
	if(nextAction) {
		GPIO_PORTA_DATA_R ^= 0x04;
	} else {
		GPIO_PORTA_DATA_R &= ~0x04;
	}
	lastStatus = SWITCH;
	NVIC_ST_RELOAD_R = reload_value;
}

int main(void){// activate grader and set system clock to 80 MHz
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();         
  EnableInterrupts();   // enable after all initialization are done
  while(1){
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
  }
}
