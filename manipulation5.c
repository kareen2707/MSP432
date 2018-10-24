#include "msp.h"

#define PERIOD 186 // to generate a period signal of ~ 1ms
#define DELAY 20 

volatile int count;

void(*TimerA0Task)(int delay);

void LED_Init(void) {
    P2->DIR = 0x10; //Selecting pin 2.4 as OUTPUT
    P2->OUT = 0x10;
}

void Toggle_LED(int delay) {
    count ++;
    if (count > delay){
        P2->OUT ^= 0x10;
        count = 0;
    }
}

void TA0_0_IRQHandler(void) {
    TIMER_A0->CCTL[0] &= ~TIMER_A_CTL_IFG;
    (*TimerA0Task)(DELAY);
}
void TimerA0_Init(void(*task) (int delay), uint16_t period) {

    TimerA0Task = task;

    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK
                    TIMER_A_CTL_ID__8 | // /8
                    TIMER_A_CTL_MC__STOP | // Stop mode
                    TIMER_A_CTL_IE; // Interrupt enable

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CM__NONE | //No capture mode
                        TIMER_A_CCTLN_CCIE; //Capture/Compare interrupt enable

    TIMER_A0->CCR[0] = (period - 1);
    TIMER_A0->EX0 &= TIMER_A_EX0_IDEX__1;;  // input clock divider /1
    NVIC->IP[2] = (NVIC->IP[2] & 0xFFFFFF00) | 0x00000040; // priority 2
    NVIC->ISER[0] = 0x00000100; // enable interrupt 8 in NVIC
    TIMER_A0->CTL |= TIMER_A_CTL_CLR | //reset the Timer
                     TIMER_A_CTL_MC_1; // start Timer A0 in up mode

}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    LED_Init();
    TimerA0_Init(&Toggle_LED, PERIOD);
    while (1) {

    }
}