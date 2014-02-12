;****************** main.s ***************
; Program written by: Your Names
; Date Created: 8/25/2013 
; Last Modified: 8/25/2013 
; Section 1-2pm     TA: Saugata Bhattacharyya
; Lab number: 1
; Brief description of the program
; The overall objective of this system is a digital lock
; Hardware connections
;  PE3 is switch input  (1 means switch is not pressed, 0 means switch is pressed)
;  PE4 is switch input  (1 means switch is not pressed, 0 means switch is pressed)
;  PE2 is LED output (0 means door is locked, 1 means door is unlocked) 
; The specific operation of this system is to 
;   unlock if both switches are pressed

;#include "TExaS.h"
;#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
;#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
;#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
;#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
;#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
;#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
;#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
;#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
;#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
;#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

;void PortF_Init(void);

;int main(void){    
  ;PortF_Init();        // Call initialization of port PF4 PF2    
  ;while(1){
		;GPIO_PORTF_DATA_R = 0x0E; // LED is white 00001110
;}
;}
;void PortF_Init(void){
  ;SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  ;GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  ;GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  ;GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  ;GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  ;GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  ;GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  ;GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  ;GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
;}
;// Color    LED(s) PortF
;// dark     ---    0
;// red      R--    0x02
;// blue     --B    0x04
;// green    -G-    0x08
;// yellow   RG-    0x0A
;// sky blue -GB    0x0C
;// white    RGB    0x0E
;// pink     R-B    0x06

; PF3-PF1 are outputs to LED3 to LED1 using positive logic

LEDS					EQU	  0x40025038
GPIO_PORTF_DATA_R 		EQU	  0x400253FC
GPIO_PORTF_DIR_R    	EQU   0x40025400
GPIO_PORTF_AFSEL_R  	EQU   0x40025420
GPIO_PORTF_PUR_R    	EQU   0x40025510
GPIO_PORTF_DEN_R    	EQU   0x4002551C
GPIO_PORTF_LOCK_R   	EQU   0x40025520
GPIO_PORTF_CR_R     	EQU   0x40025524
GPIO_PORTF_AMSEL_R  	EQU   0x40025528
GPIO_PORTF_PCTL_R   	EQU   0x4002552C
SYSCTL_RCGC2_R      	EQU   0x400FE108



      AREA    |.text|, CODE, ALIGN=2
      THUMB
      EXPORT  Start
GPIO_Init
	  ; 1. Activate clock for port F
	  LDR R1, =SYSCTL_RCGC2_R 		; load address of SYSCTL_RCGC2_R into R1
	  LDR R0, [R1] 					; load contents of R1 into R0
	  ORR R0, R0, #0x00000020 		; turn bit 5 on
	  STR R0, [R1] 					; store R0 in Port Register
	  NOP							; no operation delay
	  NOP							; no operation delay
	  
	  ;	2. No need to unlock PD3-1
	  
	  ;	allow changes to P3-P1
	  ;LDR R1, =GPIO_PORTF_CR_R 		; load address of GPIO_PORTF_CR_R into R1
	  ;LDR R0, [R1]					; load contents of R1 into R0
	  ;BIC R0, R0, #0x0000000E		; clear bits 7-0
	  ;STR R0, [R1]					; store R0 in Port Register
	  ;ORR R0, R0, #0x0000000E		; turn bits 3-1 on
	  ;STR R0, [R1]					; store R0 in Port Register
	  
	  ; 3. Disable analog functionality
	  LDR R1, =GPIO_PORTF_AMSEL_R 	; load address of GPIO_PORTF_AMSEL_R into R1
	  LDR R0, [R1]					; load contents of R1 into R0
	  BIC R0, R0, #0xFF				; clear bits PF3-PF1
	  STR R0, [R1]					; store R0 in Port Register
	  
	  ; 4. Configure as GPIO
	  LDR R1, =GPIO_PORTF_PCTL_R	; load address of GPIO_PORTF_PCTL_R into R1
	  LDR R0, [R1]					; load contents of R1 into R0
	  BIC R0, R0, #0x0000000E		; clear bits PF3-PF1
	  STR R0, [R1]					; store R0 in Port Register
	  
	  ; 5. Set direction register
	  LDR R1, =GPIO_PORTF_DIR_R 	; load address of GPIO_PORTF_DIR_R into R1
	  LDR R0, [R1]					; load contents of R1 into R0
	  ORR R0, R0, #0x0E				; make P3-P1 output
	  STR R0, [R1]					; store R0 in Port Register
	  
	  ; 6. Set regular port function
	  LDR R1, =GPIO_PORTF_AFSEL_R	; load address of GPIO_PORTF_AFSEL_R into R1
	  LDR R0, [R1]					; load contents of R1 into R0
	  BIC R0, R0, #0xFF				; disable alternate function
	  STR R0, [R1]					; store R0 in Port Register
	  
	  ; 7. enable digital port
	  LDR R1, =GPIO_PORTF_DEN_R		; load address of GPIO_PORTF_DEN_R into R1
	  LDR R0, [R1]					; load contents of R1 into R0
	  ORR R0, R0, #0x0E				; enable digital I/O on PD3-PD1
	  STR R0, [R1]					; store R0 in Port Register
	  
	  BX LR							; return

Start
	  BL GPIO_Init					; jump to subroutine GPIO_Init
	  LDR R0, =LEDS					; R0 = LEDS
	  MOV R1, #2					; red
	  MOV R2, #4					; blue
	  MOV R3, #8					; green
	  MOV R4, #13					; white
	  
loop
	  ;STR R1, [R0]
	  ;STR R2, [R0]
	  ;STR R3, [R0]
	  STR R4, [R0]
	  B   loop


      ALIGN        ; make sure the end of this section is aligned
      END          ; end of file
		  
		  