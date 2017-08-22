#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void Delay_s(uint32_t count);
void LED_flashing(uint32_t flash_time);

int main(void){
    //Step 1 - Unlock protected registers
    SYS_UnlockReg();

    //Step 2 - Enable clock source
    CLK->PWRCON |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);          //PWRCON - System power down control register
                                    //pin[1] = 1 -> enable XTL32K_EN - External 32.768 kHz LOW Speed Crystal Enable 
                                    //HXT - External High frequency Crystal 12 MHz
    while(!(CLK->CLKSTATUS & 1<<0));  //Check if the clock is stable yet before moving on

    //Step 3 - Select the clock source
    CLK->CLKSEL0 &= ~(0x07<<0);      //Clear all the clock selection before choose new one
    CLK->CLKSEL0 |= (0x07<<0);       //Chose HXT

    //Step 4 - Divide the Clock(optional)
    CLK->CLKDIV &= ~(0x0F);         //clear all previous clock division setting
    CLK->CLKDIV |= (0x07 << 0);
	
    //Step 5 - Lock protected registers
    SYS_LockReg();

    //Main function
    PC->PMD |= (0x01) << 24;        //configure PC12 as Output
    while(1) {
        //your code here
        LED_flashing(5*2);
    }
}

void Delay_s(uint32_t count) {
    uint32_t n;
    for (n = 0; n < count; n++){

    }
}

void LED_flashing(uint32_t flash_time){
    uint32_t i;
    for (i = 0; i < (flash_time*2); i++){
        PC->DOUT ^= 1 << 12;        //Toggle PC12
        Delay_s(1000);
    }
}