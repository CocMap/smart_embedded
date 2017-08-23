#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"


void Delay_s(uint32_t count);
void ISR_function(void);
void Interrupt_bip_time(uint32_t bip_time);

int main(void){
    //--------------------CONFIGURE INTERRUPT PB15--------------------
    PB->PMD &= ~(0x03) << 30;             //configure PB15 interrupt button
    PB->PMD |= (0x00) << 30;              //configure PB15 as Input

    PB->IMD &= ~ (1<<15);              //set 0 for PB15 -> Edge trigger interrupt
    PB->DBEN |= (1<<15);             //enable the de-bounce function (use for Edge trigger only)
    GPIO->DBNCECON |= (1<<4);          //choose the Clock source (internal 10kHz)
    PB->IEN |= (1<<15);              //enable interrupt Falling Edge

    NVIC->ISER[0] |= (1<<3);            //set control register -> PB15 external signal inerrupt

    //--------------------CONFIGURE LED5 (NORMANL OPERATION)--------------------
    PC->PMD &= ~(0x03)<<24;             //configure PC12 led5
    PB->PMD |= (0x01)<<24;              //configure PC12 as output

    //--------------------CONFIGURE LED8 (ISR)---------------------
    //PC->PMD &= ~(0x03)<<30;             //configure PC15 leb8
    PC->PMD |= (1<<30);              //configure PC15 as output

    //--------------------MAIN FUNCTION (NORMAL OPEARTION)---------------------
    while(1){
        PC->DOUT ^=1 <<12;              //toggle PC12 leb5
        Delay_s(1000000);
    }
}

void ISR_function(void){
    if(PB->ISRC & (1ul<<15)){          //if interrupt button PB15 pressed (interrupt source flag)
        PB->ISRC = PB->ISRC;
        Interrupt_bip_time(5);
    }

    NVIC->ICPR[0] |= (1ul<<3);            //clear-pending interrupt PB15 
}

void Delay_s(uint32_t count){
    uint32_t n;
    for(n=0; n<count; n++){ }
}

void Interrupt_bip_time(uint32_t bip_time){
    uint32_t i;
    for(i=0; i<bip_time; i++){
        PC->DOUT ^=1 <<15;              //toggle PC15 led8
        Delay_s(2000000);
    }
}

