#define PERIOD 3750 // to generate a period signal of ~ 20ms

void TimerA0_Delay(uint16_t period) {

    P2->DIR = 0x10; // Selectig P2.4 as output
    P2->SEL0 |= 0x10; //Asociating Timer0A function to P2.4 pin
    P2->SEL1 &= ~0x10;

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK
                    TIMER_A_CTL_ID__8 | // /8
                    TIMER_A_CTL_MC__UP; // Up/Down mode

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_OUTMOD_1; // OUTMOD:Toggle
    TIMER_A0->EX0 = TIMER_A_EX0_IDEX__1; //input CLK divider /1;
    TIMER_A0->CCR[0] = period-1;
}
/**
* main.c
*/
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    TimerA0_Delay(PERIOD);
    while (1) {

    }

}