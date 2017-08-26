#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define DELAYTIME 1000000
#define FLASHTIME 5
#define BIPTIME 3
#define BIPDELAY 200000
#define CLOCKSOURCE 0 //0: HXT-12MHz; 1: LXT-32kHz; 2: HIRC-22MHz; 3: LIRC-10kHz
<<<<<<< HEAD
#define HXT_CLK 1ul << 0
#define LXT_CLK 1ul << 1
#define HIRC_CLK 1ul << 2
#define LIRC_CLK 1ul << 3
#define CLK_SEL_MASK (~(0x07 << 0))
=======
#define HXT_CLK 1ul<<0
#define LXT_CLK 1ul<<1
#define HIRC_CLK 1ul<<2
#define LIRC_CLK 1ul<<3
#define CLK_SEL_MASK (~(0x07<<0))
>>>>>>> 8f6eb0cb920ab48cee9dd1a4db22b0403c996f53
#define HXT_CLK_SEL 0x00
#define LXT_CLK_SEL 0x01
#define PLL_CLK_SEL 0x02
#define HIRC_CLK_SEL 0x07
#define LIRC_CLK_SEL 0x03
Page 5
<<<<<<< HEAD
#define HXT_STATUS 1ul << 0
#define LXT_STATUS 1ul << 1
#define PLL_STATUS 1ul << 2
#define HIRC_STATUS 1ul << 4
#define LIRC_STATUS 1ul << 3
#define CLK_SW_STATUS 1ul << 7
#define PLLCON_FB_DV_MASK (~(0x01FFul << 0))
#define PLLCON_FB_DV_VAL 44
#define CPUCLOCKDIVIDE 8
    void
    Delay_s(uint32_t count);
=======
#define HXT_STATUS 1ul<<0
#define LXT_STATUS 1ul<<1
#define PLL_STATUS 1ul<<2
#define HIRC_STATUS 1ul<<4
#define LIRC_STATUS 1ul<<3
#define CLK_SW_STATUS 1ul<<7
#define PLLCON_FB_DV_MASK (~(0x01FFul << 0))
#define PLLCON_FB_DV_VAL 44
#define CPUCLOCKDIVIDE 8
void Delay_s(uint32_t count);
>>>>>>> 8f6eb0cb920ab48cee9dd1a4db22b0403c996f53
void LED_flashing(uint32_t flash_time);
void Buzzer_bip(uint32_t bip_time);
void CLK_SW_check(void);
int main(void)
{
<<<<<<< HEAD
    SYS_UnlockReg(); // Unlock protected registers

    // Enable clock sources
    //HXT - External High frequency Crystal 12 MHz
    CLK->PWRCON |= HXT_CLK;
    while (!(CLK->CLKSTATUS & HXT_STATUS))
        ;

    //LXT - External Low frequency Crystal 32 kHz
    CLK->PWRCON |= LXT_CLK;
    while (!(CLK->CLKSTATUS & LXT_STATUS))
        ;

    //PLL configuration starts
    //CLK->PLLCON |= 1ul<<19; //0: PLL input is HXT 12MHz (default). 1: PLL input is HIRC 22MHz
    CLK->PLLCON &= ~(1ul << 18); //0: enable PLL clock out. 1: disable PLL clock (default)
    CLK->PLLCON &= ~(1ul << 16); //0: PLL in normal mode. 1: PLL in power-down mode (default)
    CLK->PLLCON &= PLLCON_FB_DV_MASK;
    CLK->PLLCON |= PLLCON_FB_DV_VAL; //frequency: 1 MHz * (PLLCON_FB_DV_VAL+2)
    while (!(CLK->CLKSTATUS & PLL_STATUS))
        ;

    //clock source selection
    CLK->CLKSEL0 &= CLK_SEL_MASK;
    CLK->CLKSEL0 |= PLL_CLK_SEL;

    //clock division
    CLK->CLKDIV &= ~0x0F;
    CLK->CLKDIV |= (CPUCLOCKDIVSIDE - 1);
    //PLL configuration ends

    SYS_LockReg(); // Lock protected registers
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    while (1)
    {
        //your code here
        LED_flashing(FLASHTIME * 2);
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
=======
 SYS_UnlockReg(); // Unlock protected registers

 // Enable clock sources
 //HXT - External High frequency Crystal 12 MHz
 CLK->PWRCON |= HXT_CLK;
 while(!(CLK->CLKSTATUS & HXT_STATUS));

 //LXT - External Low frequency Crystal 32 kHz
 CLK->PWRCON |= LXT_CLK;
 while(!(CLK->CLKSTATUS & LXT_STATUS));

 //PLL configuration starts
 //CLK->PLLCON |= 1ul<<19; //0: PLL input is HXT 12MHz (default). 1: PLL input is HIRC 22MHz
 CLK->PLLCON &= ~(1ul<<18); //0: enable PLL clock out. 1: disable PLL clock (default)
 CLK->PLLCON &= ~(1ul<<16); //0: PLL in normal mode. 1: PLL in power-down mode (default)
 CLK->PLLCON &= PLLCON_FB_DV_MASK;
 CLK->PLLCON |= PLLCON_FB_DV_VAL; //frequency: 1 MHz * (PLLCON_FB_DV_VAL+2)
 while(!(CLK->CLKSTATUS & PLL_STATUS));

 //clock source selection
 CLK->CLKSEL0 &= CLK_SEL_MASK;
 CLK->CLKSEL0 |= PLL_CLK_SEL;

 //clock division
 CLK->CLKDIV &= ~0x0F;
 CLK->CLKDIV |= (CPUCLOCKDIVSIDE-1);
 //PLL configuration ends

 SYS_LockReg(); // Lock protected registers
 GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
 while(1){
//your code here
 LED_flashing(FLASHTIME*2);
 }
}

void Delay_s(uint32_t count){
 uint32_t n;
 for(n=0;n<count;n++){
 }
 }
void LED_flashing(uint32_t flash_time){
 uint32_t i;
 for(i=0;i<(flash_time*2);i++){
 PC12 ^= 1;
 Delay_s(DELAYTIME);
 }
 }
>>>>>>> 8f6eb0cb920ab48cee9dd1a4db22b0403c996f53
//------------------------------------------- main.c CODE ENDS ---------------------------------------------------------------------------