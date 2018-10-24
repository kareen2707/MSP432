#include "msp.h"
#include <stdint.h>
#include <stdio.h>

#define RED       0x01
#define GREEN     0x02
#define BLUE      0x04
#define WHEELSIZE 8           // must be an integer power of 2
//    red, yellow,    green, light blue, blue, purple,   white,          dark
const long COLORWHEEL[WHEELSIZE] = { RED, RED + GREEN, GREEN, GREEN + BLUE, BLUE, BLUE + RED, RED + GREEN + BLUE, 0 };
volatile uint32_t ADCvalue;

void ADC0_InitSWTRiggerCh6(void) {
	while (REF_A->CTL0 & 0x0400) {}; // wait for the reference to be idle before reconfiguring

	REF_A->CTL0 = 0x0039; // REF_A_CTL0_VSEL_3 | REF_A_CTL0_TCOFF | REF_A_CTL0_ON
						  // 15-14 reserved                          00b (reserved)
						  // 13    REFBGRDY   bandgap voltage ready   0b (read only)
						  // 12    REFGENRDY  reference voltage ready 0b (read only)
						  // 11    BGMODE     bandgap mode            0b (read only)
						  // 10    REFGENBUSY no reconfigure if busy  0b (read only)
						  // 9     REFBGACT   bandgap active          0b (read only)
						  // 8     REFGENACT  reference active        0b (read only)
						  // 7     REFBGOT    trigger bandgap         0b = no software trigger set
						  // 6     REFGENOT   trigger reference       0b = no software trigger set
						  // 5-4   REFVSEL    voltage level select   11b = 2.5V
						  // 3     REFTCOFF   temperature disable     1b = temperature sensor disabled to save power
						  // 2     reserved                           0b (reserved)
						  // 1     REFOUT     reference output buffer 0b = reference output not on P5.6 (see also REFBURST in ADC14CTL1, P5SEL1, and P5SEL0)
						  // 0     REFON      reference enable        1b = reference enabled in static mode

	while ((REF_A->CTL0 & 0x1000 /*REF_A_CTL0_GENRDY*/) == 0) {}; // wait for the reference to stabilize before continuing

	ADC14->CTL0 &= ~ADC14_CTL0_ENC; // 2) ADC14ENC = 0 to allow programming
	while (ADC14->CTL0 & ADC14_CTL0_BUSY) {};  //wait for BUSY to be zero
	ADC14->CTL0 = ADC14_CTL0_PDIV_0 | //Predivide by 1
		ADC14_CTL0_SHS_0 | //ADC14SC bit
		ADC14_CTL0_SHP |  // SAMPCON the sampling timer
		ADC14_CTL0_DIV__1 | // /1
		ADC14_CTL0_SSEL__SMCLK | // SMCLK
		ADC14_CTL0_CONSEQ_0 | //Single-channel, single-conversion
		ADC14_CTL0_SHT1__32 | //Sample and hold time 32 CLKs
		ADC14_CTL0_SHT0__32 | //Sample and hold time 32 CLKs
		ADC14_CTL0_ON; //powered up

	ADC14->CTL1 = 0x00000030; //ADC14_CTL1_RES__14BIT; 14 bit, 16 clocks Puede faltar configurar STARTADDx

	ADC14->MCTL[0] = ADC14_MCTLN_INCH_6 | //A6, P4.7
		ADC14_MCTLN_EOS | //End of Sequence
		ADC14_MCTLN_VRSEL_1; // V(R+) = VREF, V(R-) = AVSS
	ADC14->IER0 = 0; //No interrupts
	ADC14->IER1 = 0;

	P4->SEL0 |= 0x80;                  // 8) analog mode on A6, P4.7
	P4->SEL1 |= 0x80;

	ADC14->CTL0 |= ADC14_CTL0_ENC; // Enable

}

uint32_t ADC_In(void) {

	while (ADC14->CTL0 & ADC14_CTL0_BUSY) {}; // wait for Busy to be zero

	ADC14->CTL0 |= ADC14_CTL0_SC; // starts single conversion

	while ((ADC14->IFGR0 & ADC14_IFGR0_IFG0) == 0) {};

	return ADC14->MEM[0];
}


int main(void) {

	//ADC0_InitSWTriggerCh6();         // initialize ADC sample P4.7/A6

	P1->SEL0 &= ~0x01;
	P1->SEL1 &= ~0x01;                 // configure built-in LED1 as GPIO
	P1->DIR |= 0x01;                   // make built-in LED1 out
	P1->OUT &= ~0x01;                  // LED1 = off
									   // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in RGB LEDs)
	P2->SEL0 &= ~0x07;
	P2->SEL1 &= ~0x07;                 // configure built-in RGB LEDs as GPIO
	P2->DS |= 0x07;                    // make built-in RGB LEDs high drive strength
	P2->DIR |= 0x07;                   // make built-in RGB LEDs out
	P2->OUT &= ~0x07;                  // RGB = off
									   //EnableInterrupts();
	while (1) {
		// update the color wheel according to the ADC measurement
		P2OUT = (P2OUT&~0x07) | (COLORWHEEL[(ADCvalue >> 11)&(WHEELSIZE - 1)]);
		//WaitForInterrupt();
	}
}
