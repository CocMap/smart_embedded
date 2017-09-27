#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define HXT_CLK 1ul << 0
#define LXT_CLK 1ul << 1
#define HIRC_CLK 1ul << 2
#define LIRC_CLK 1ul << 3
#define CLK_SEL_MASK (~(0x07 << 0))
#define HXT_CLK_SEL 0x00
#define LXT_CLK_SEL 0x01
#define PLL_CLK_SEL 0x02
#define HIRC_CLK_SEL 0x07
#define LIRC_CLK_SEL 0x03
#define HXT_STATUS 1ul << 0
#define LXT_STATUS 1ul << 1
#define PLL_STATUS 1ul << 2
#define HIRC_STATUS 1ul << 4
#define LIRC_STATUS 1ul << 3
#define CLK_SW_STATUS 1ul << 7
#define PLLCON_FB_DV_MASK (~(0x01FFul << 0))
#define PLLCON_FB_DV_VAL 48
#define CPUCLOCKDIVIDE 1
#define DELAYTIME 1000000 << 2
#define FLASHTIME 5

void Delay_s(uint32_t count);
void LED_flashing(uint32_t flash_time);

int main(void)
{
    //--------------------------------
    //System initialization
    //--------------------------------
    SYS_UnlockReg();
    CLK->PWRCON |= HXT_CLK;
    while (!(CLK->CLKSTATUS & HXT_STATUS));
    CLK->PWRCON |= LXT_CLK;
    while (!(CLK->CLKSTATUS & LXT_STATUS));

    //PLLCON = PLL control register
    CLK->PLLCON &= ~(1ul << 18);            //148   //clear bit 18 -> PPLOUT fix low            
    CLK->PLLCON &= ~(1ul << 16);                    //clear bit 16 -> PLL is in power down mode (default)
    CLK->PLLCON &= PLLCON_FB_DV_MASK;               //clear 9 bits [0~8] -> clear PLL Feedback Divider Control Pins 
    CLK->PLLCON |= PLLCON_FB_DV_VAL;                //set 48Mhz
    while (!(CLK->CLKSTATUS & PLL_STATUS));

    //CLKSEL0 = for HCLK and CPU STCLK (system tick) clock select
    CLK->CLKSEL0 &= CLK_SEL_MASK;           //142   //clear bits [0][1][2]  
    CLK->CLKSEL0 |= PLL_CLK_SEL;                    //set 010 -> clock source from PPL clock

    CLK->CLKDIV &= ~0x0F;                   //147   //clear bit 0~3
    CLK->CLKDIV |= (CPUCLOCKDIVIDE - 1);            //set 0

    CLK->CLKSEL1 &= ~(7ul << 20);           //143   //clock select control register 1
                                                    //clear bit 20~22
    CLK->CLKSEL1 |= 2ul << 20;                      //set 010 for TMR3 -> clock source from HCLK
    CLK->APBCLK |= 1ul << 5;                        //APBCLK = APB Devices Clock Enable Control Register
                                                    //set 1 at bit 5 -> timer3 clock enable
    SYS_LockReg();
    //--------------------------------
    // LED5 flashing
    //--------------------------------
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    //--------------------------------
    //Timer3 initialization
    //--------------------------------
    SYS->GPB_MFP |= 1ul << 11;              //71    //GPIOB Multiple Function and Input Type Control Register 
                                                    //set 01 for bit 11 -> GPB11 become TMR
    SYS->ALT_MFP &= ~(1ul << 4);            //80    //Alternative Multiple Function Pin Control Register 
    TIMER3->TCSR |= 1ul << 26;              //313   //TCSR - timer control and status register
                                                    //set 1 at bit26 -> Timer reset bit
                                                    //Reset Timer 8-bit pre-scaler counter, internal 24-bit up-timer and CEN bit 
    TIMER3->TCSR &= ~(1ul << 29);                   //disable timer interrupt
    TIMER3->TCSR |= 3ul << 27;                      //set 11 continuous counting mode
    TIMER3->TCSR |= 1ul << 24;                      //Event counter enable
    TIMER3->TCSR |= 1ul << 16;                      //data load enable -> data will be update continously

    TIMER3->TCMPR = 50;                     //314   //Timer compare register
    TIMER3->TEXCON |= 1ul << 7;             //318   //Timer external control register
                                                    //enable Timer counter pin de-bounce enable bit
    TIMER3->TEXCON &= ~(1ul << 0);                  //falling edg of external count pin
    TIMER3->TCSR |= 1ul << 30;                      //Timer enable -> start counting

    //-------------------------------------------------------------------------------------------------------------
    //Main program
    //-------------------------------------------------------------------------------------------------------------
    while (1)
    {
        if (TIMER3->TDR == 5)
        {
            LED_flashing(TIMER3->TDR);
            TIMER3->TCSR |= 1ul << 26; //reset TIMER3
            TIMER3->TCSR |= 1ul << 30; //start counting
        }
    }
}
void Delay_s(uint32_t count)
{
    uint32_t n;
    for (n = 0; n < count; n++)
    {
    }
}
void LED_flashing(uint32_t flash_time)
{
    uint32_t i;
    for (i = 0; i < (flash_time * 2); i++)
    {
        PC12 ^= 1;
        Delay_s(DELAYTIME);
    }
}
