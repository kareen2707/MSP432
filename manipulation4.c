#include "msp.h"

#define PERIOD 3750 // to generate a period signal of ~ 20ms
#define DUTY_CYCLE 70
#define PERIOD_ON (PERIOD*DUTY_CYCLE)/100

void PWM_Init(uint16_t period, uint16_t period_on) {

    P2->DIR = 0x10; // Selectig P2.4 as output
    P2->SEL0 |= 0x10; //Asociating Timer0A function to P2.4 pin
    P2->SEL1 &= ~0x10;

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK
                    TIMER_A_CTL_ID__8 | // /8
                    TIMER_A_CTL_MC__UPDOWN; // Up/Down mode

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_OUTMOD_4; // OUTMOD:Toggle
    TIMER_A0->EX0 = TIMER_A_EX0_IDEX__1; //input CLK divider /1;
    TIMER_A0->CCR[0] = period;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_2; // OUTMOD:Toggle/Reset
    TIMER_A0->CCR[1] = period_on;
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    PWM_Init(PERIOD, PERIOD_ON);
    while (1) {

    }

}