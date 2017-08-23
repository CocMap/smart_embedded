//------------------------------------------- main.c CODE STARTS ---------------------------------------------------------------------------
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void Delay_s(uint32_t count);
void EINT1_IRQHandler(void);
void Buzzer_bip(uint32_t bip_time);

int main(void)
{
    //--------------------SYSTEM INITIALIZATION--------------------
    uint32_t i = 0;
    SYS_Init();

    //--------------------CONFIGURE OUTPUT - Led PC12 & Buzzer PB11 --------------------
    //LED display via PC12 to indicate main program execution
    PC->PMD &= ~(0x03) << 24;                   //clear current 24~25 bit
    PC->PMD |= (0x01) << 24;                    //configre output mode for LED PC12
                                                
    //BUZZER to indicate interrupt handling routine
    PB->PMD &= ~(0x03) << 22;                   //configure output mode for buzzer PB11 (use standard_driver)
    PB->PMD |= (0x03) << 23;                    //configure as output

    //LED display via PC15 - LED8 to show ISR is turned on
    PC->PMD &= ~(0x03) << 30;                   //configure output for led8
    PC->PMD |= ~(0x03) << 30;                   //configure output for led8  

    //--------------------CONFIGURE INTERRUPT--------------------
    //Step 1 - Choose the Interrupt pin -> configure input (PB15)
    PB->PMD &= ~(0x03) << 30;                   //clear current 24~25 bit
    PB->PMD |= (0x00) << 30;                    //configure 00 as input

    //Step 2 - Set LEVEL or EDGE trigger interrupt
    PB->IMD &= ~(0x01<<15);                     //IMD = 0 -> EDGE interrupt for PB15 
                                                //if the interrupt is EDGE trigger, the trigger source can be
                                                //controlled by de-bounce -> step 3

                                                //if the interrupt is LEVEL trigger, input source is sampled
                                                //by one HCLK and generates the interrupt -> step 4 
    PB->IEN |= (0x01<<15);                      //detect FALLING EDGE
                                                //enable PB15
    NVIC->ISER[0] |= (0x0l<<3);                 //ENABLE EXTERNAL interrupt

    while (1)
    {
        PC->DOUT ^= 1 << 12;
        Delay_s(100000);
    }
}

void EINT1_IRQHandler(void){
    if (PB->ISRC & (0x01<<15)){                 //ISRC = interrupt source flag
        PB->ISRC = PB->ISRC;
        Buzzer_bip(5); //buzzer bip 5 times
        PC->DOUT ^=1 <<15;
        Delay_s(100000);
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
        PB11 ^= 1;
        Delay_s(2000000); //Buzzer bip deplay time
    }
}
//------------------------------------------- main.c CODE ENDS ---------------------------------------------------