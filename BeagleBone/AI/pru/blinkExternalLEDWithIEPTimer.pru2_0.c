////////////////////////////////////////
//  blinkExternalLEDWithIEPTimer.c
//  Blinks one LED wired to P8_11 by writing to memory using the PRU
//  Wiring:  P8_11 connects to the plus lead of an LED.  The negative lead of the
//      LED goes to a 220 Ohm resistor.  The other lead of the resistor goes
//      to ground.
//  Setup:  Set the direction to out on P8_11
//  See:  prugpio.h to see to which ports the P8 and P9 pins are attached
//  PRU:  pr2_0 or pr2_1
////////////////////////////////////////
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_iep.h>
#include "resource_table_empty.h"
#include "prugpio.h"

volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define WAIT 500000000

// http://www.ti.com/lit/ug/spruhz6l/spruhz6l.pdf
// Section 30.1.11.2.2.3 PRU-ICSS Industrial Ethernet Timer Basic Programming Sequence
void setup_iep() {
  // 1. Initialize timer to known state (default values)
  CT_IEP.GLB_CFG_bit.CNT_ENABLE = 0;
  CT_IEP.LOW_COUNTER = 0xffffffff;
  CT_IEP.HIGH_COUNTER = 0xffffffff;
  CT_IEP.GLB_STS_bit.CNT_OVF = 1;
  CT_IEP.CMP_STS = 0xffffffff;

  // 2. Set compare values
  CT_IEP.CMP0_0 = WAIT;
  CT_IEP.CMP0_1 = 0;

  // 3. Enable compare events
  CT_IEP.CMP_CFG_bit.CMP_EN = 1;

  // 4. Set increment value
  CT_IEP.GLB_CFG_bit.DEFAULT_INC = 5;

  // 5. Set compensation value
  CT_IEP.COMPEN_bit.COMPEN_CNT = 0;
  CT_IEP.SLOW_COMPEN = 0;

  // 6. Enable counter
  CT_IEP.GLB_CFG_bit.CNT_ENABLE = 1;
}

void reset_iep() {
  CT_IEP.CMP_STS = 0xffffffff;
}

int read_iep_hit() {
  return CT_IEP.CMP_STS_bit.CMP_HIT & 0x1;
}

#define P8_11 (1<<11)
void main() {
  // Points to the GPIO port that is used
  uint32_t *gpio3 = (uint32_t *)GPIO3;

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  setup_iep();

  while(1) {
    reset_iep();
    while(!read_iep_hit()) {}
//    __delay_cycles(WAIT/5);
    gpio3[GPIO_SETDATAOUT]   = P8_11;  // Turn the USR1 LED on

    reset_iep();
    while(!read_iep_hit()) {}
//    __delay_cycles(WAIT/5);
    gpio3[GPIO_CLEARDATAOUT] = P8_11;  // Off

  }
  __halt();
}

// Set direction of P8_11 (which is port 3 pin 11)
#pragma DATA_SECTION(init_pins, ".init_pins")
#pragma RETAIN(init_pins)
const char init_pins[] =  
  "/sys/class/gpio/export\0 75\0" \
  "/sys/class/gpio/gpio75/direction\0out\0" \
  "\0\0";

