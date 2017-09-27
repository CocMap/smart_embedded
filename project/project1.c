#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

unsigned long pulseIn(uint32_t bit, uint32_t port, unsigned long timeout);

void main(void) {
    //configure ECHO = GPC0 -> input; TRIGGER = GPC1 -> output
    GPIO_SetMode(PC,BIT12,GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC,BIT13,GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC,BIT14,GPIO_MODE_OUTPUT);

    PC->PMD &= ~(1111<<0);				//clear 0~3 
                                        //configure input 00 for ECHO pin GPC0
    PC->PMD |= 1<<2;                    //configure output 01 for TRIGGER pin GPC1
   
    long duration = 0;
    long distance = 0;

    while (1) {
        //initial setup for TRIGGER     
        PC->DOUT = 0<<1;
        CLK_SysTickDelay(2);            //delay 2us 
        
        //supply a short 10uS pulse to the trigger input to start the ranging
        PC->DOUT = 1<<1;
        CLK_SysTickDelay(10);           //10us to start 

        //turn off and wait
        PC->DOUT = 0<<1;

        duration = pulseIn(PC, 0x00, 1);
        distance = duration*0.034/2;
    }
}

unsigned long pulseIn(GPIO_T *port, uint32_t u32PinMask, uint32_t state, unsigned long timeout) {
    uint32_t stateMask = (state ? u32PinMask : 0);         //u32PinMask = bit 
    unsigned long width = 0;            //keep initial width = 0

    //convert timeout from microsecond to a number of times through
    //the initial loop take 16 cycle per iteration
    unsigned long num_loop = 0;
    unsigned long max_loop = microsecondsToClockCycle(timeout)/16;

    while(port->u32PinMask == stateMask)
        if(num_loop++ == max_loop)
            return 0;

    while(port->u32PinMask != stateMask)
        if(num_loop++ == max_loop)
            return 0;

    while(port->u32PinMask == stateMask) {
        if(num_loop++ == max_loop)
            return 0;
        width++;
    }

    return clockCycleToMicrosecond(width*21+16);
}

void microsecondToClockCycle(unsigned long time) {     
    int clockCycle;
    clockCycle = (0.04*time);
    return clockCycle;
}

void clockCycleToMicrosecond(unsigned long clockCycle) {
    int microsecond;
    microsecond = (clockCycle/0.04);
    return microsecond;
}




