#include "msp.h"

#define FREQ 70
#define PERIOD 1000000/FREQ
#define DUTY_CYCLE 40
#define PERIOD_ON (PERIOD*DUTY_CYCLE)/100
#define PERIOD_OFF (PERIOD*(100-DUTY_CYCLE))/100

void int main(int argc, char const *argv[]) {

  int i;
  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

  P2->DIR=0xFF;

  while (1){

    P2->OUT=0x01; //0x10 To write in the P2.5 pin
    for (i = 0; i < PERIOD_ON; i++);
    P2->OUT=0x00;
    for (i = 0; i < PERIOD_OFF; i++);
  }

  return 0;
}
