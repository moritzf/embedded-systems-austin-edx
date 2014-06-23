// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

unsigned char SineWave[TABLE_SIZE] = {8,9,11,12,13,14,14,15,15,15,14,14,13,12,11,9,8,7,5,4,3,2,2,1,1,1,2,2,3,4,5,7};
unsigned char i = 0;
unsigned char isOff;

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
  DAC_Init();
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 0x0000FFFF;
	NVIC_ST_CURRENT_R = 0;
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000;  // priority 0
	NVIC_ST_CTRL_R = 0x0007;
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
	isOff = 0;
// this routine sets the RELOAD and starts SysTick
	NVIC_ST_RELOAD_R = (80000000 / (period * TABLE_SIZE)) - 1;
	i = 0;
}

// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
	DAC_Out(0);
	isOff = 1;
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	if(!isOff) {
	i = (i + 1) & 0x1F;
	DAC_Out(SineWave[i]);
	}
}

void Sound_Play(unsigned char note) {
	switch(note) {
		case 'C': {
			Sound_Tone(523.251);
		}; break;
		case 'D': {
			Sound_Tone(587.330);
		}; break;
		case 'E': {
			Sound_Tone(659.255);
		}; break;
		case 'G': {
			Sound_Tone(783.991);
		}; break;
	}
}