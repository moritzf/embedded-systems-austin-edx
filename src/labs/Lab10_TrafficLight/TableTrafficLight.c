// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
#define CLIGHTS (*((volatile unsigned long *) 0x400050FC))
#define SENSORS (*((volatile unsigned long *) 0x4002401C))
#define PLIGHTS (*((volatile unsigned long *) 0x40025028))
#define GoW 0
#define WaitW 1
#define GoS 2
#define WaitS 3
#define GoP 4
#define WaitP1 5
#define WaitP2 6
#define WaitP3 7
#define WaitP4 8

typedef const struct State StateT;
struct State {
	unsigned char lights;
	unsigned long wait;
	unsigned char next[8];
	};

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void sysTickTimer_Init(void);
void ports_Init(void);
void portB_Init(void);
void portE_Init(void);
void portF_Init(void);
void delay10ms(unsigned long);
void sysTickWait(unsigned long);
void init(void);
void lights_out(unsigned char);
unsigned char traffic_in(void);
// ***** 3. Subroutines Section *****

int main(void){
		
	
	StateT FSM[9] = {
	{0x31, 100, {GoW, GoW, WaitW, WaitW, WaitW, WaitW, WaitW, WaitW}},
	{0x51, 50, {GoS, GoS, GoS, GoS, GoP, GoP, GoS, GoS}},
	{0x85, 100, {GoS, WaitS, GoS, WaitS, WaitS, WaitS, WaitS, WaitS}},
	{0x89, 50, {GoP, GoW, GoP, GoW, GoP, GoW, GoP, GoP}},
	{0x92, 100, {WaitP1, WaitP1, WaitP1, WaitP1, GoP, WaitP1, WaitP1, WaitP1}},
	{0x90, 50, {WaitP2, WaitP2, WaitP2, WaitP2, WaitP2, WaitP2, WaitP2, WaitP2}},
	{0x92, 50, {WaitP3, WaitP3, WaitP3, WaitP3, WaitP3, WaitP3, WaitP3, WaitP3}},
	{0x90, 50, {WaitP4, WaitP4, WaitP4, WaitP4, WaitP4, WaitP4, WaitP4, WaitP4}},
	{0x90, 50, {GoW, GoW, GoS, GoW, GoW, GoW, GoS, GoW}},	
	};
	
	unsigned long state;
	init();
	
  while(1){
    lights_out(FSM[state].lights);
		delay10ms(FSM[state].wait);
		state = FSM[state].next[traffic_in()];
  }
}

void ports_Init(void) {
	portB_Init();
	portE_Init();
	portF_Init();
}

void init(void) {
	TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
	EnableInterrupts();
	sysTickTimer_Init();
	ports_Init();
}

void sysTickTimer_Init(void) {
	NVIC_ST_CTRL_R = 0; // 1) disable SysTick during setup
	NVIC_ST_RELOAD_R = 0x00FFFFFF; // 2) maximum reload value
	NVIC_ST_CURRENT_R = 0; // 3) any write to current clears it
	NVIC_ST_CTRL_R = 0x00000005; // 4) enable SysTick with core clock
}

void sysTickWait(unsigned long delay) { // delay is in 12.5 ns units
	NVIC_ST_RELOAD_R = delay-1; // number of counts to wait
	NVIC_ST_CURRENT_R = 0; // any value written to CURRENT clears
	while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
		}
}

void delay10ms(unsigned long delay) {
	sysTickWait(800000 * delay);
}
void portB_Init(void) {
	SYSCTL_RCGC2_R |= 0x00000002;
	sysTickWait(100);
	GPIO_PORTB_AMSEL_R &= 0x00;
	GPIO_PORTB_PCTL_R &= 0x00000000;
	GPIO_PORTB_DIR_R |= 0x0000003F;
	GPIO_PORTB_AFSEL_R &= 0x00;
	GPIO_PORTB_PUR_R &= 0x00;
	GPIO_PORTB_DEN_R |= 0x3F;
}

void portF_Init(void) {
	SYSCTL_RCGC2_R |= 0x00000020;
	sysTickWait(100);
	GPIO_PORTF_AMSEL_R &= 0x00;
	GPIO_PORTF_PCTL_R &= 0x00000000;
	GPIO_PORTF_DIR_R |= 0x000000A;
	GPIO_PORTF_AFSEL_R &= 0x00;
	GPIO_PORTF_PUR_R &= 0x00;
	GPIO_PORTF_DEN_R |= 0x0A;
}

void portE_Init(void) {
	SYSCTL_RCGC2_R |= 0x00000010;
	sysTickWait(100);
	GPIO_PORTE_AMSEL_R &= 0x00;
	GPIO_PORTE_PCTL_R &= 0x00000000;
	GPIO_PORTE_DIR_R |= 0x00;
	GPIO_PORTE_AFSEL_R &= 0x00;
	GPIO_PORTE_PUR_R &= 0x00;
	GPIO_PORTE_DEN_R |= 0x07;
}

unsigned char traffic_in(void) {
	 return SENSORS&0x07;
}

void lights_out(unsigned char output) {
	CLIGHTS = ((output&0xFC)>>2);
	PLIGHTS = ((output&0x02)<<2) + ((output&0x01)<<1);
}