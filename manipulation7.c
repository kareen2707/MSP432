#include "msp.h"

#define PERIOD 3750
#define MAX_ADC_VAL 16383
#define LIMIT 5700

void(*FinishedTask)(uint16_t result); //user function called when conversion complete


void task(uint16_t result) {

	P1->OUT ^= 0x01; // toggle P1.0 built-in LED1

	int difference;

	TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK
		TIMER_A_CTL_ID__8 | // /1
		TIMER_A_CTL_MC__UPDOWN; // Up-Down Mode

	TIMER_A2->CCTL[0] = 0x0080; // TIMER_A_CCTLN_OUTMOD_4 OUTMOD:Toggle
	TIMER_A2->EX0 = TIMER_A_EX0_IDEX__1; //
	TIMER_A2->CCR[0] = PERIOD;
	TIMER_A2->CCTL[1] = 0x0040; // TIMER_A_CCTLN_OUTMOD_2 OUTMOD:Toggle/Reset

	difference = MAX_ADC_VAL - result;
	if (difference >= LIMIT) {
		TIMER_A2->CCR[1] = 0;
	}
	else TIMER_A2->CCR[1] = 3000;
}

// ***************** ADC0_InitTA0TriggerCh6 ****************
/* Activate Timer A0 to periodically trigger ADC conversions
on P4.7 = A6. Run the user task when each conversion is complete.
Inputs:  task is a pointer to a user function
period in units (1/SMCLK), 16 bits
Outputs: none
*/

void ADC0_InitTA0TriggerCh6(void(*task)(uint16_t result), uint16_t period) {
	FinishedTask = task;
	TIMER_A0->CTL &= TIMER_A_CTL_MC_MASK;
	TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK
		TIMER_A_CTL_ID__8 | // antes /1
		TIMER_A_CTL_MC__STOP; // Stop mode

	TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CM__NONE | //No capture mode
		TIMER_A_CCTLN_OUTMOD_3; //Set/Reset output mode

	TIMER_A0->CCR[0] = (period - 1);// defines when output signal is cleared
	TIMER_A0->CCR[1] = (period - 1) / 2;// defines when output signal is set, triggering ADC conversion

	TIMER_A0->EX0 = TIMER_A_EX0_IDEX__1; //input CLK divider /1

	while (REF_A->CTL0 & REF_A_CTL0_GENBUSY) {}; // wait for the reference to be idle before reconfiguring

												 //    REF_A->CTL0 = REF_A_CTL0_VSEL_3 |  // voltage level select=2.5V
												 //                  REF_A_CTL0_TCOFF | // temperature sensor disabled to save power
												 //                  REF_A_CTL0_ON; // reference enabled in static mode

	REF_A->CTL0 = 0x0039;

	while ((REF_A->CTL0 & REF_A_CTL0_GENRDY) == 0) {}; // wait for the reference to stabilize before continuing

	ADC14->CTL0 &= ~ADC14_CTL0_ENC; // 2) ADC14ENC = 0 to allow programming

	while (ADC14->CTL0 & ADC14_CTL0_BUSY) {};  //wait for BUSY to be zero

	ADC14->CTL0 = ADC14_CTL0_PDIV_0 | // Pre-divided by 1
		ADC14_CTL0_SHS_1 | //TIMER_A0 bit
		ADC14_CTL0_SHP |  // SAMPCON the sampling timer
		ADC14_CTL0_DIV__1 | // /8 antes /1
		ADC14_CTL0_SSEL__SMCLK | // SMCLK
		ADC14_CTL0_CONSEQ_2 | //Repeat single channel
		ADC14_CTL0_SHT1__16 | //Sample and hold time 32 CLKs
		ADC14_CTL0_SHT0__16 | //Sample and hold time 32 CLKs
		ADC14_CTL0_ON; //powered up

	ADC14->CTL1 = ADC14_CTL1_RES__14BIT;

	ADC14->MCTL[0] = ADC14_MCTLN_VRSEL_1 | // V(R+) = VREF, V(R-) = AVSS
		ADC14_MCTLN_EOS | //End of Sequence
		ADC14_MCTLN_INCH_6;//A6, P4.7

	ADC14->IER0 |= ADC14_IER0_IE0; //enable ADC14IFG0 interrupt
	ADC14->IER1 = 0; //disable ADC14IFG1 interrupt

	P4->SEL0 |= 0x80; // 8) analog mode on A6, P4.7
	P4->SEL1 |= 0x80;

	ADC14->CTL0 |= ADC14_CTL0_ENC; // Enable

	NVIC->IP[6] = (NVIC->IP[6] & 0xFFFFFF00) | 0x00000040; // priority 2
	NVIC->ISER[0] = 0x01000000; // enable interrupt 24 in NVIC

	TIMER_A0->CTL |= TIMER_A_CTL_MC_1 | TIMER_A_CTL_CLR; // reset and start Timer A0 in up mode

}

void ADC14_IRQHandler(void) {
	uint16_t value_readed;
	if ((ADC14->IFGR0 & ADC14_IFGR0_IFG0) == ADC14_IFGR0_IFG0) {
		value_readed = ADC14->MEM[0];
		(*FinishedTask)(value_readed);
	}
}

int main(void) {


	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
	P5->DIR = 0x40; // Selectig P5.6 as output
	P5->SEL0 |= 0x40; //Asociating Timer0A function to P5.6 pin
	P5->SEL1 &= ~0x40;
	ADC0_InitTA0TriggerCh6(&task, 90000);// initialize ADC sample P4.7/A6, 12,000,000/60,000 = 200 Hz
										 // initialize P1.0 and make it output (P1.0 built-in LED1)
	P1->SEL0 &= ~0x01;
	P1->SEL1 &= ~0x01;                 // configure built-in LED1 as GPIO
	P1->DIR |= 0x01;                   // make built-in LED1 out
	P1->OUT &= ~0x01;                  // LED1 = off



	__enable_interrupt();
	while (1) {
		//WaitForInterrupt();

	}
}
