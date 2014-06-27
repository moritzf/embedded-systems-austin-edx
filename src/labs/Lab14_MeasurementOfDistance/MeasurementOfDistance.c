// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
// April 8, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE1 and PD3
// Slide pot pin 1 connected to ground


#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"
#include "math.h"
#define CEIL(a, b) (((a) / (b)) + (((a) % (b)) > 0 ? 1 : 0))
// Calibration constants
#define CALIB1 2
#define CALIB2 0
#define DEBUG_MONITOR GPIO_PORTF_DATA_R

void Debug_Init(void);
void EnableInterrupts(void);  // Enable interrupts

unsigned char String[10];
unsigned long Distance; // units 0.001 cm
unsigned long ADCdata;  // 12-bit 0 to 4095 sample
unsigned long Flag;     // 1 means valid Distance, 0 means Distance is empty

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
// Constant: 0.001
unsigned long Convert(unsigned long sample){
	Distance = CEIL(sample*2000, 4095);
	return Distance;
}

//double round(double d)
//{
//  return floor(d + 0.5);
//}
// Debugging monitor initialization
void Debug_Init(void) {
	unsigned volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTF_DEN_R |= 0x02;
	GPIO_PORTF_DIR_R |= 0x02;
}
// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0; // 1) disable SysTick during setup
	NVIC_ST_RELOAD_R = period - 1; // 2) maximum reload value
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; //priority 2
	NVIC_ST_CURRENT_R = 0; // 3) any write to current clears it
	NVIC_ST_CTRL_R = 0x07; // 4) enable SysTick with core clock
}
// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){
	int i;
	for(i = 0; i < 2; i++) {
	DEBUG_MONITOR ^= 0x02;
	}
	Convert(ADC0_In());
	Flag = 1;
	DEBUG_MONITOR ^= 0x02;
}

//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n){
	int index = 4;
	int i;
	if(n <= 9999) {
	String[0] = '0';
	String[1] = '.';
	for(i = 2; i < 5; i++) {
		String[i] = '0';
	}
	
	while(n >= 10) {
		if(index == 1) index--;
		String[index] = (n % 10) + 0x30;
			n = n / 10;
			index--;
	}
	if(index == 1) index--;
	String[index] = (n % 10) + 0x30;
	index--;
	} else {
		String[0] = '*';
		String[1] = '.';
		for(i = 2; i < 5; i++) {
			String[i] = '*';
		}
	}
	String[5] = ' ';
	String[6] = 'c';
	String[7] = 'm';
}
int main(void){ 
   TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
	ADC0_Init(); // initialize ADC0, channel 1, sequencer 3
	Nokia5110_Init(); // initialize Nokia5110 LCD (optional)
	SysTick_Init(2000000); // initialize SysTick for 40 Hz interrupts
	Debug_Init(); // initialize profiling on PF1 (optional)
                                    //    wait for clock to stabilize
  EnableInterrupts();
	Nokia5110_OutString("LAB 14\0");
  while(1){
		Flag = 0;
		while(Flag == 0) {};
		UART_ConvertDistance(Distance);
		Nokia5110_Clear();
		Nokia5110_OutString("Distance:");
		Nokia5110_SetCursor(0, 2);
		Nokia5110_OutString(String);
  }
}

int main1(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
  }
}
int main2(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_NoScope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  Nokia5110_Init();             // initialize Nokia5110 LCD
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
    Nokia5110_SetCursor(0, 0);
    Distance = Convert(ADCdata);
    UART_ConvertDistance(Distance); // from Lab 11
    Nokia5110_OutString(String);    
  }
}

