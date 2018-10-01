#include "msp.h"

#define PERIOD 10000 // to generate a period signal of ~ 20ms

void LED_Init(void) {
	P2->DIR = 0x01;
}

void LED_ON (void) {
	P2->OUT = 0x01;
}

void LED_OFF(void) {
	P2->OUT = 0x00;
}

void TimerA0_Delay(uint16_t period) {

	TIMER_A0->CTL = 0x0290; 
	TIMER_A0->CCTL[0] = 0x0010;
	TIMER_A0->CCR[0] = (period - 1);
	TIMER_A0->EXO = 0x0005;
}
/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
	LED_Init();

	while (1) {
		LED_ON();
		TimerA0_Delay(PERIOD);
		LED_OFF();
	}
	
}
