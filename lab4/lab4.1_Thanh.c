#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void SystemTickDelay(void);
int main(void){
    //Step 1 - Unlock protected registers
    SYS_UnlockReg();

    //Step 2 - Enable Clock Source
    CLK->PWRCON |= (0x0F) << 0;         //turn on all clock source (2 external, 2 internal)
    while(!(CLK->CLKSTATUS & 1<<0));    //check if clock is stable yet before moving on

    //Step 3 - Select the clock source
    CLK->CLKSEL0 &= ~(0xFF << 0);       //clear all clock selection before choose a new one
    CLK->CLKSEL |= (0x01 << 0);         //choose the 32.768 LOW speed crystal clock

    //Step 4 - Divide the clock (optional)

    //Step 5 - Lock protected registers
    SYS_LockReg();

}