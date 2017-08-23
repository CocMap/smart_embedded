#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void Delay_s(uint32_t count);
void EINT1_IRQHandler(void);
void Buzzer_bip(uint32_t bip_time);

int main(void)
{
    //--------------------CONFIGURE OUTPUT FOR THE MAIN FUNCTION --------------------
    //LED display via PC12 to indicate main program execution
    PC->PMD &= ~(0x03) << 24; //clear current 24~25 bit
    PC->PMD |= (0x01) << 24;  //configre output mode for LED PC12

    //--------------------CONFIGURE INTERRUPT--------------------
    //Step 1 - Choose the Interrupt pin -> configure input (PB15)
    PB->PMD &= ~(0x03) << 30; //clear current 24~25 bit
    PB->PMD |= (0x00) << 30;  //configure 00 as input

    //Step 2 - Set LEVEL or EDGE trigger interrupt
    PB->IMD &= ~(1 << 15); //IMD = 0 -> EDGE interrupt for PB15
                           //if the interrupt is EDGE trigger, the trigger source can be
                           //controlled by de-bounce 

                            //if the interrupt is LEVEL trigger, input source is sampled
                            //by one HCLK and generates the interrupt 
    PB->DBEN |= (1<<15);        //enable de-bounce 
    GPIO->DBNCECON |= 1 << 4;    //de-bounce counter clock source - internal 10kHz
    PB->IEN |= (1 << 15);      //detect FALLING EDGE
                               //enable PB15
    

    NVIC->ISER[0] |= (1 << 3); //ENABLE EXTERNAL interrupt


    // PC->PMD |= (0x01) << 30;  //configre output mode for LED PC12

    //--------------------CONFIGURE OUTPUT FOR THE ISR --------------------
   

    //LED display via PC15 - LED8 to show ISR is turned on
   PB->PMD |= (0x01) << 22; //configure output for led8

    while (1)
    {
        PC->DOUT ^= 1 << 12;
        Delay_s(100000);
    }
}

void EINT1_IRQHandler(void)
{
    if (PB->ISRC & (1ul << 15))
    { //ISRC = interrupt source flag
        PB->ISRC = PB->ISRC;
        Buzzer_bip(5); //buzzer bip 5 times
    }
    //NVIC->ICPR[0] |= 1ul << 3;                  //clear-pending control
}

void Delay_s(uint32_t count)
{
    uint32_t n;
    for (n = 0; n < count; n++)
    {
    }
}

void Buzzer_bip(uint32_t bip_time)
{
    uint32_t i;
    for (i = 0; i < (bip_time * 2); i++)
    {
        PC->DOUT ^= 1 << 15;
        PB11 ^= 1;
        Delay_s(200000); //Buzzer bip deplay time
        //PB->DOUT ^= 0 << 11;
    }
}
//------------------------------------------- main.c CODE ENDS ---------------------------------------------------