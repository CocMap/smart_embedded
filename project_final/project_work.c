#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "sensor_function.h"
#include "LCD_function.h"

char display[100];

//------------------------------------------------------------------------------------------------------------------------------------
// Main program
//------------------------------------------------------------------------------------------------------------------------------------
int main(void)
{

    //configure LCD
    System_Config();
    SPI3_Config();
    LCD_start();
    LCD_clear();

    //configure sensor and timer2
    HCSR04_GPIO_Init();
    TIMER2_Init();
    Interrupt_Init();

    while (1)
    {
        HCSR04_Trigger();
        if (Capture == 1)
        {
            LCD_clear();
            //distance = (0.034*duration)/(2);
            distance = duration / 58; //......................Oh well, datasheet... :|
            snprintf(display, sizeof(display), "Distance: %d", distance);
            printLine(0, display);
            Capture = 0;
        }
        Delay_us(60000); // Delay
    }
}


//----------------------------------Delay_us--------------------------------------//
void Delay_us(uint32_t count)
{
    uint32_t n;
    for (n = 0; n < (count / 3); n++)
    {
    }
}

