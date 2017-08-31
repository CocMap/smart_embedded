#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#define DELAYTIME 1000000       // 1M unit
#define BUZZER_BIPTIME 1        // Bip 5 times
#define BUZZER_BIPDELAY 2000000 // 2M unit

void Delay_s(uint32_t count);
void Buzzer_bip(uint32_t bip_time);
void GPAB_IRQHandler(void);

void OpenKeyPad(void)
{
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5), GPIO_MODE_QUASI);
    GPIO_EnableInt(PA, 0, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 1, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 2, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 3, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 4, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 5, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPAB_IRQn);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);
    GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5));
}

uint8_t ScanKey(void)
{
    PA0 = 1;
    PA1 = 0;
    PA2 = 1;
    PA3 = 1;
    PA4 = 1;
    PA5 = 1;
    if (PA3 == 0)
        return 2;
    if (PA4 == 0)
        return 5;
    if (PA5 == 0)
        return 8;
    PA0 = 1;
    PA1 = 1;
    PA2 = 0;
    PA3 = 1;
    PA4 = 1;
    PA5 = 1;
    if (PA3 == 0)
        return 1;
    if (PA4 == 0)
        return 4;
    if (PA5 == 0)
        return 7;

    PA0 = 0;
    PA1 = 1;
    PA2 = 1;
    PA3 = 1;
    PA4 = 1;
    PA5 = 1;
    if (PA3 == 0)
        return 3;
    if (PA4 == 0)
        return 6;
    if (PA5 == 0)
        return 9;
    return 0;
}
void Init_GPIO(void)
{
    GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
    PB11 = 1;
    //	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
}

int main(void)
{
    uint8_t i = 0;
    SYS_Init();
    OpenKeyPad();
    Init_GPIO();

    // Start Loop
    while (1)
    {
        switch (i)
        {
        case 0:
            PA0 = 1;
            PA1 = 1;
            PA2 = 0;
            PA3 = 1;
            PA4 = 1;
            PA5 = 1;
            break;
        case 1:
            PA0 = 1;
            PA1 = 0;
            PA2 = 1;
            PA3 = 1;
            PA4 = 1;
            PA5 = 1;
            break;
        case 2:
            PA0 = 0;
            PA1 = 1;
            PA2 = 1;
            PA3 = 1;
            PA4 = 1;
            PA5 = 1;
            break;
        default:
            PA0 = 1;
            PA1 = 1;
            PA2 = 1;
            PA3 = 1;
            PA4 = 1;
            PA5 = 1;
            break;
            i++;
            if (i > 2)
                i = 0;
        }
        //	PC12 ^= 1;
        //	Delay_s(DELAYTIME);
    }
}
void GPAB_IRQHandler(void)
{ // Most important function

    if (PA->ISRC & (1ul << 3))
    {
        Buzzer_bip(BUZZER_BIPTIME * 1);
        PA->ISRC = PA->ISRC;
    }
    if (PA->ISRC & (1ul << 4))
    {
        Buzzer_bip(BUZZER_BIPTIME * 4);
        PA->ISRC = PA->ISRC;
    }
    if (PA->ISRC & (1ul << 5))
    {
        Buzzer_bip(BUZZER_BIPTIME * 7);
        PA->ISRC = PA->ISRC;
    }

    NVIC->ICPR[0] |= 1ul << 4; // Remove pending state of associated interrupt
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
        Delay_s(BUZZER_BIPDELAY);
    }
}
