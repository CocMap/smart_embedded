#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

// #define HXT_CLK 1ul << 0
// #define LXT_CLK 1ul << 1
// #define HIRC_CLK 1ul << 2
// #define LIRC_CLK 1ul << 3
// #define CLK_SEL_MASK (~(0x07 << 0))
// #define HXT_CLK_SEL 0x00
// #define LXT_CLK_SEL 0x01
// #define PLL_CLK_SEL 0x02
// #define HIRC_CLK_SEL 0x07
// #define LIRC_CLK_SEL 0x03
// #define HXT_STATUS 1ul << 0
// #define LXT_STATUS 1ul << 1
// #define PLL_STATUS 1ul << 2
// #define HIRC_STATUS 1ul << 4
// #define LIRC_STATUS 1ul << 3
// #define CLK_SW_STATUS 1ul << 7
// #define PLLCON_FB_DV_MASK (~(0x01FFul << 0))
// #define PLLCON_FB_DV_VAL 30
// #define CPUCLOCKDIVIDE 1
// #define DELAYTIME 1000000 // 1M unit

// #define    _SR04A_ECHO           (GPB_15)            //NUC140VE3xN, Pin91/PB.15/INT1
// #define    _SR04A_TRIG           (GPB_4)            //NUC140VE3xN, Pin19/PB.4/RXD1
// #define    _SR04A_TRIG_Low     (GPB_4 = 0)
// #define    _SR04A_TRIG_High (GPB_4 = 1)

// #define rising 1
// #define falling 2
// #define both 3

// void TIMER2_Start(void);
// void TIMER2_Stop(void);
// void TIMER2_Init(void);

// char TEXT1[16] = "D =     mm";

// volatile uint32_t SRF04_Echo_Width = 0;
// volatile uint32_t distance = 0;
// volatile uint32_t  INT1_Trigger;
// volatile uint32_t SRF04_Echo_Flag  = FALSE;

int main (void)
{
	System_Config();
    SRF04_GPIO_Init();
    TIMER2_Init();
    DrvGPIO_SetDebounceTime(8, E_DBCLKSRC_HCLK);    //????????????????

   while(1)
    {
       
        SRF04_Trigger();
        DrvSYS_Delay(40000); //??????????????????????????
        if(SRF04_Echo_Flag == 1)
        {
            distance = (343*SRF04_Echo_Width)/(2000);
					
					/////////////////////Change to our LCD code
            sprintf(TEXT1+6, " %d mm  \n", distance);
            printf(TEXT1) ;
					//////////////////////////END
            SRF04_Echo_Flag = 0;
            DrvSYS_Delay(50000);	//////////////////////
            DrvSYS_Delay(50000);	////////////////////////
        }
       
    }
}

void System_Config(void)
{
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

    //PLL
    //CLK->PLLCON |= 1ul<<19;   //0: PLL input is HXT 12MHz (default). 1: PLL input is HIRC 22MHz
    CLK->PLLCON &= ~(1ul << 18); //0: enable PLL clock out. 1: disable PLL clock (default)
    CLK->PLLCON &= ~(1ul << 16); //0: PLL in normal mode. 1: PLL in power-down mode (default)
    CLK->PLLCON &= PLLCON_FB_DV_MASK;
    CLK->PLLCON |= PLLCON_FB_DV_VAL; //frequency: 1 MHz * (PLLCON_FB_DV_VAL+2)
    while (!(CLK->CLKSTATUS & PLL_STATUS));

    //clock source selection
    CLK->CLKSEL0 &= CLK_SEL_MASK;
    CLK->CLKSEL0 |= PLL_CLK_SEL;

    //clock division
    CLK->CLKDIV &= ~0x0F;
    CLK->CLKDIV |= (CPUCLOCKDIVIDE - 1);

		//Timer 2 initialise
		CLK->CLKSEL1 &= ~(7ul << 16); //Clear clock selection for TIMER2
		CLK->CLKSEL1 &= ~(0x7 << 16); //Choose 12MHz clock for TIMER2
		CLK->APBCLK |= 1ul << 4; // TMR2_EN
		
    //enable clock of SPI3
    CLK->APBCLK |= 1ul << 15;

    SYS_LockReg(); // Lock protected registers
}

//---------------------------------------------------------------------------------TIMER2
// Timer Clock:    12 MHz
// Prescale:        11
// Compare:        0xffffff
// Mode:            Period atm
// void TIMER2_Init(void)
// {
//     TIMER2->TCSR &= ~(3ul << 27);
// 		TIMER2->TCSR &= ~(3ul << 27); // PERIOD OR ONE_SHOT????

//     TIMER2->TCSR |= (0xB << 0);    // TIME2 frequency = 12/(11+1) = 1Mhz
//     TIMER2->TCMPR = 0xffffff;        // Set TCMPR is 24 bits register [0~16777215]
                               
//     /* Step 4. Disable interrupt */
//     TIMER2->TCSR &= ~(1 << 29);
//     TIMER2->TISR |= 1ul << 0;        //Write 1 to clear for safety       

//     TIMER2->TCSR &= ~(1 << 30);        //Disable Timer2

// //      TIMER2->TCSR.TDR_EN=1;        // Enable TDR function
// }

// void TIMER2_Start(void)
// {       
//     TIMER2->TCSR |= (0xB << 0);    // TIME2 frequency = 12/(11+1) = 1Mhz
//     TIMER2->TCMPR = 0xffffff;        // Set TCMPR is 24 bits register [0~16777215]
//     TIMER2->TCSR |= (1 << 30);        //Start Counting Timer2
   
// }

// void TIMER2_Stop(void)
// {
//     TIMER2->TCSR |= 1ul << 26;        //Reset TIMER2   
//     TIMER2->TCSR &= ~(1 << 30);        //Disable Timer2
// }







