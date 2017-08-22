#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void SystemTickDelay(void);
int main(void){
    //--------------------SETUP CLOCK--------------------
    //Step 1 - Unlock protected registers
    SYS_UnlockReg();

    //Step 2 - Enable Clock Source
    CLK->PWRCON |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);         //turn on all clock source (2 external, 2 internal)
    while(!(CLK->CLKSTATUS & 1<<0));    //check if clock is stable yet before moving on

    //Step 3 - Select the clock source
    CLK->CLKSEL0 &= ~(0xFF << 0);       //clear all clock selection before choose a new one
    CLK->CLKSEL0 |= (0x01 << 0);         //choose the 32.768 LOW speed crystal clock

    //Step 4 - Divide the clock (optional)

    //Step 5 - Lock protected registers
    SYS_LockReg();

    //--------------------SYSTEM CLOCK CONFIGURATION--------------------
    //
    SysTick->CTRL |= 1<<2;              //the SysTick shares same clock with CPU
    //SysTick->CTRL &= ~(1<<1);         //turn off the SysTick interrupt
    // SysTick->CTRL &= ~(1<<1);           //the SysTick has an independent clock to the clock of the CPU
    SysTick->CTRL |= (1<<1);            //turn ON systick interrupt

    SysTick->LOAD = 32768 - 1;          //set the top value -> 500mHz
    SysTick->LOAD = 49;                 //set the top value -> 500mHz
    SysTick->VAL = 0;                   //clear the current value to 0

    PC->PMD |= (0x01) << 24;            //configure PC12 as output
    SysTick->CTRL |= 1<<0;              //enable the timer (time start to count)

    //--------------------MAIN LOOP--------------------
    while(1){
        //while(!(SysTick->CTRL & 1<<16));//check if the COUNTLFLAG (bit 16) is set
        PC->DOUT ^=1 << 12;             //toggle PC12
    }
}

// void SysTick_Handler(void){
//     PC->DOUT ^= 1 << 12;                //toggle PC12
// }






