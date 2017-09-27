#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD_library.h"
#include "timer_interrupt_library.h"

//-----------------------------call functions
// void TIMER2_Start(void);
// void TIMER2_Stop(void);
// void TIMER2_Init(void);

// void SRF04_GPIO_Init(void);
// void SRF04_Trigger(void);
// void SRF04_Distance_Display(uint32_t distance); // hien thi LCD

// void Interrup_Config(void);
// void Edge_Falling_Enable(void);
// void Edge_Rising_Enable(void);
// void Interrupt_Disable(void);
// void Set_Debounce(void);

//-----------------------------call variables
volatile uint32_t SRF04_Echo_Width = 0;
volatile uint32_t distance = 0;
// volatile uint32_t INT1_Trigger;
volatile uint32_t SRF04_Echo_Flag = FALSE;

char display[30];

//-----------------------------declare functions
int main(void)
{
    SRF04_GPIO_Init();
    TIMER2_Init();
    //Set_Debounce();

    System_Config();

    SPI3_Config();

    LCD_start();
    LCD_clear();

    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    while (1)
    {
        PC12 = 0;                 // turn on LED, PORT C 12, led5
        CLK_SysTickDelay(100000); // Delay
        PC12 = 1;                 // turn off LED
        CLK_SysTickDelay(100000); // Delay

        printS_5x7(0, 8, "TRan");
        SRF04_Trigger();
        CLK_SysTickDelay(40000);

        if (SRF04_Echo_Flag == 1)
        {
            distance = (343 * SRF04_Echo_Width) / (2000);

            /////////////////////Change to our LCD code
            sprintf(TEXT1 + 6, " %d mm  \n", distance);
            LCD_clear();

            //snprintf(display, sizeof(display), "Distance: %d", distance);
            //printS_5x7(10, 8, display);
            //CLK_SysTickDelay(200000);

            printS_5x7(10, 8, TEXT1);
            //printS_5x7(10, 8, distance);
            //////////////////////////END
            SRF04_Echo_Flag = 0;
            CLK_SysTickDelay(50000); //////////////////////
            CLK_SysTickDelay(50000); ////////////////////////
        }
    }
}


