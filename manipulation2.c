#include "msp.h"

#define PERIOD 1000

void int main(int argc, char const *argv[]) {

  int i;
  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

  P2->DIR=0xFF;

  while (1){

    P2->OUT=0x01; //To write in the P2.0 pin
    for (i = 0; i < PERIOD; i++);
    P2->OUT=0x00;
    P2->OUT=0x02; //To write in the P2.1 pin
    for (i = 0; i < PERIOD; i++);
    P2->OUT=0x00;
    P2->OUT=0x04; //To write in the P2.2 pin
    for (i = 0; i < PERIOD; i++);
    P2->OUT=0x00;

  }

  return 0;
}
