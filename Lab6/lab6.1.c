#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define HXT_CLK 
#define LXT_CLK 
#define HIRC_CLK 1ul << 2
#define LIRC_CLK 1ul << 3
#define CLK_SEL_MASK 
#define HXT_CLK_SEL 0x00
#define LXT_CLK_SEL 0x01
#define PLL_CLK_SEL 
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
    void
    SPI_transmit(unsigned int temp);
int main(void)
{
    //--------------------------------
    //System initialization
    //--------------------------------
    SYS_UnlockReg(); // Unlock protected registers
    // Enable clock sources
    //HXT - External High frequency Crystal 12 MHz
    CLK->PWRCON |= 1ul << 0;                        //turn on clock source HXT 
    while (!(CLK->CLKSTATUS & HXT_STATUS));          //check if the clock is stable yet before moving on
        
    //LXT - External Low frequency Crystal 32 kHz
    CLK->PWRCON |= 1ul << 1;                        //turn on clock source LXT
    while (!(CLK->CLKSTATUS & LXT_STATUS));
        

    //PLL
    //CLK->PLLCON |= 1ul<<19; //0: PLL input is HXT 12MHz (default). 1: PLL input is HIRC 22MHz
    CLK->PLLCON &= ~(1ul << 18); //0: enable PLL clock out. 1: disable PLL clock (default)
    CLK->PLLCON &= ~(1ul << 16); //0: PLL in normal mode. 1: PLL in power-down mode (default)
    CLK->PLLCON &= PLLCON_FB_DV_MASK;
    CLK->PLLCON |= PLLCON_FB_DV_VAL; //frequency: 1 MHz * (PLLCON_FB_DV_VAL+2)
    while (!(CLK->CLKSTATUS & PLL_STATUS));
        ;

    //clock source selection
    CLK->CLKSEL0 &= (~(0x07 << 0));                 //select clock source 22.1184 MHz
    CLK->CLKSEL0 |= 0x02;                           //select PPL clock source

    //clock division
    CLK->CLKDIV &= ~0x0F;                           
    CLK->CLKDIV |= (CPUCLOCKDIVIDE - 1);
    
    //enable clock of SPI3
    CLK->APBCLK |= 1ul << 15;                       //APB device clock enable SPI13 

    SYS_LockReg(); // Lock protected registers
    //--------------------------------
    //SPI3 initialization
    //--------------------------------
    SYS->GPD_MFP |= 1ul << 11;                      //Multiple function pin PD11 -> set 1 to choose SPI3 (Master out, slave in)
    SYS->GPD_MFP |= 1ul << 9;                       //Multiple function pin PD11 -> set 1 to choose SPI3 (SPICLK)
    SYS->GPD_MFP |= 1ul << 8;                       //Multiple function pin PD11 -> set 1 to choose SPI3 (SS30)

    SPI3->CNTRL &= ~(1ul << 23); //0: disable variable clock feature
    SPI3->CNTRL &= ~(1ul << 22); //0: disable two bits transfer mode
    SPI3->CNTRL &= ~(1ul << 18); //0: select Master mode
    SPI3->CNTRL &= ~(1ul << 17); //0: disable SPI interrupt

    SPI3->CNTRL &= ~(1ul << 11);       //295   //SPI control and status register
                                            //pin11 (CLKP) = 1 SPICLK idle high

    SPI3->CNTRL &= ~(1ul << 10);    //295   //MSB first

    SPI3->CNTRL &= ~(3ul << 8);     //296   //TX_NUM only one transmit word
    SPI3->CNTRL &= ~(31ul << 3);            //clear first 3~731ul = 0x1F -> 31 bits
    SPI3->CNTRL |= 12ul << 3;               //12ul = 1100 -> 12bits
    SPI3->CNTRL |= (1ul << 2);      //296   //1: transmit at Falling edge

    SPI3->DIVIDER = 24;             //297   //master only
                                            //
    // SPI3->SSR |= (1ul<<2);          //select active high
    SPI_transmit(0xCDEB);
}
void SPI_transmit(unsigned int temp)
{
    SPI3->SSR |= 1ul << 0;              //299   //Activate the slave select signal
    SPI3->TX[0] = temp;                 //
    SPI3->CNTRL |= 1ul << 0;            //      //GO-BUSY = 1 ->start data transmition       
    while (SPI3->CNTRL & (1ul << 0));   //      //check if it transmittion finished
    SPI3->SSR &= ~(1ul << 0);                   //inactivate the slave select signal
}