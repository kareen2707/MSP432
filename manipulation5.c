#include "msp.h"

#define PERIOD 10000 // to generate a period signal of ~ 20ms

void LED_Init(void) {
	P2->DIR = 0x01;
}

void Toggle_LED (void) {
	P2->OUT ^= 0x01;
}

void TA0_0_IRQHandler(void) {
	TIMER_A0->CCTL[0] &= ~0x0001;
	*TimerA0Task();
}
void TimerA0_Init(void(*task) (void), uint16_t period) {
	TimerA0Task = task;
	TIMER_A0->CTL = 0x0280; 
	TIMER_A0->CCTL[0] = 0x0010;
	TIMER_A0->CCR[0] = (period - 1);
	TIMER_A0->EXO = 0x0005;
	NVIC->IP[2] = (NVIC->IP[2] & 0xFFFFFF00) | 0x00000040;
	NVIC->ISER[0] = 0x00000100;
	TIMER_A0->CTL |= 0x0014;
}
/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
	LED_Init();
	TimerA0_Init(&Toggle_LED, PERIOD);

	while (1) {

	}
	
}
