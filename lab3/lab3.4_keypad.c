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
void OpenKeyPad(void);
uint32_t ScanKey(void);
void Init_GPIO(void);
void led_bip(uint32_t bip_time);

int main(void)
{
    
    SYS_Init();
    OpenKeyPad();
    Init_GPIO();

    //Start Loop
    while (1)
    {
        PC12 = 0;                 // turn on LED, PORT C 12, led5
        CLK_SysTickDelay(100000); // Delay
        PC12 = 1;                 // turn off LED
        CLK_SysTickDelay(100000); // Delay
        // i = ScanKey();          //scan which key is pressed
        // switch (i) {
        // case 1: Buzzer_bip(1); break;
        //     // PA0 = 1;
        //     // PA1 = 1;
        //     // PA2 = 0;
        //     // PA3 = 1;
        //     // PA4 = 1;
        //     // PA5 = 1;
        //     // break;
        // case 2: Buzzer_bip(2); break;
        //     // PA0 = 1;
        //     // PA1 = 0;
        //     // PA2 = 1;
        //     // PA3 = 1;
        //     // PA4 = 1;
        //     // PA5 = 1;
        //     // break;
        // case 3: Buzzer_bip(3); break;
        // case 4: Buzzer_bip(4); break;
        // case 5: Buzzer_bip(5); break;
        // case 6: Buzzer_bip(6); break;
        // case 7: PC12 = 0; break;
        // case 8: PC13 = 0; break;
        // case 9: PC14 = 0; break;

        //     // PA0 = 0;
        //     // PA1 = 1;
        //     // PA2 = 1;
        //     // PA3 = 1;
        //     // PA4 = 1;
        //     // PA5 = 1;
        //     // break;
        // default: PC15 = 0; break;

        //     // PA0 = 1;
        //     // PA1 = 1;
        //     // PA2 = 1;
        //     // PA3 = 1;
        //     // PA4 = 1;
        //     // PA5 = 1;
        //     // break;
        //     // i++;
        //     // if (i > 2)
        //     //     i = 0;
    }
    //	PC12 ^= 1;
    //	Delay_s(DELAYTIME);
}

void GPAB_IRQHandler(void)
{ // Most important function
    uint32_t i = 0;
    i = ScanKey();

    switch (i) {
    case 1: 
        PA->ISRC & (1ul << 3);
        PA->ISRC & (1ul << 2);  

        led_bip(BUZZER_BIPTIME * 1);
        PA->ISRC = PA->ISRC;
        break;

    case 2: 
        PA->ISRC & (1ul << 1);
        PA->ISRC & (1ul << 2);  

        led_bip(BUZZER_BIPTIME * 3);
        PA->ISRC = PA->ISRC;
        break;
    
    case 3: 
        PA->ISRC & (1ul << 0);
        PA->ISRC & (1ul << 3);  

        led_bip(BUZZER_BIPTIME * 1);
        PA->ISRC = PA->ISRC;
        break;

    case 4: 
        PA->ISRC & (1ul << 2);
        PA->ISRC & (1ul << 4);  

        led_bip(BUZZER_BIPTIME * 3);
        PA->ISRC = PA->ISRC;
        break;

    case 5: 
        PA->ISRC & (1ul << 1);
        PA->ISRC & (1ul << 4);  

        led_bip(BUZZER_BIPTIME * 1);
        PA->ISRC = PA->ISRC;
        break;

        // case 2: 
        // PA->ISRC & (1ul << 1);
        // PA->ISRC & (1ul << 2);  

        // Buzzer_bip(BUZZER_BIPTIME * 3);
        // PA->ISRC = PA->ISRC;
        // break;

        // case 2: 
        // PA->ISRC & (1ul << 1);
        // PA->ISRC & (1ul << 2);  

        // Buzzer_bip(BUZZER_BIPTIME * 3);
        // PA->ISRC = PA->ISRC;
        // break;

        // case 2: 
        // PA->ISRC & (1ul << 1);
        // PA->ISRC & (1ul << 2);  

        // Buzzer_bip(BUZZER_BIPTIME * 3);
        // PA->ISRC = PA->ISRC;
        // break;

    default:
    PA->ISRC & (1ul << 0);
    PA->ISRC & (1ul << 5);  
    Buzzer_bip(4);
    PA->ISRC = PA->ISRC;
    break;



    // if (PA->ISRC0 & (1ul << 3))
    // {
    //     Buzzer_bip(BUZZER_BIPTIME * 4);
    //     PA->ISRC = PA->ISRC;
    // }

    // if (PA->ISRC & (1ul << 4))
    // {
    //     Buzzer_bip(BUZZER_BIPTIME * 4);
    //     PA->ISRC = PA->ISRC;
    // }

    // if (PA->ISRC & (1ul << 4))
    // {
    //     Buzzer_bip(BUZZER_BIPTIME * 4);
    //     PA->ISRC = PA->ISRC;
    // }
    // if (PA->ISRC & (1ul << 5))
    // {
    //     Buzzer_bip(BUZZER_BIPTIME * 7);
    //     PA->ISRC = PA->ISRC;
    // }

    
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

void led_bip(uint32_t bip_time)
{
    uint32_t i;
    for (i = 0; i < (bip_time * 2); i++)
    {
        PC15 ^= 1;
        Delay_s(BUZZER_BIPDELAY);
    }
}

void OpenKeyPad(void) //configure keypad + configure interrupt for keypad
{
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5), GPIO_MODE_INPUT);
    GPIO_EnableInt(PA, 0, GPIO_INT_FALLING); //egde trigger falling edge
    GPIO_EnableInt(PA, 1, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 2, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 3, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 4, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 5, GPIO_INT_FALLING);

    GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5)); //enable de-bounce
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);        //debounce clock source 10kHz

    NVIC_EnableIRQ(GPAB_IRQn); //enable external interrupt
}

uint32_t ScanKey(void)
{
    //set the keypad position (choose col first, then row)
    //choose col -> set 0
    //choose col 1 (set PA2 = 0, the rest disable = 1)
    PA0 = 1;
    PA1 = 1;
    PA2 = 0;
    PA3 = 1;
    PA4 = 1;
    PA5 = 1;
    //choose row
    if (PA3 == 0)
        return 1; //PA3 enable -> key K1
    if (PA4 == 0)
        return 4; //PA4 enable -> key K4
    if (PA5 == 0)
        return 7; //PA5 enable -> key K7

    //choose col 2 (set PA1 = 0, the rest disable = 1)
    PA0 = 1;
    PA1 = 0;
    PA2 = 1;
    PA3 = 1;
    PA4 = 1;
    PA5 = 1;
    if (PA3 == 0)
        return 2; //PA3 enable -> key K2
    if (PA4 == 0)
        return 5; //PA4 enable -> key K4
    if (PA5 == 0)
        return 8; //PA5 enable -> key K7

    //choose col 3 (set PA0 = 0, the rest disable = 1)
    PA0 = 0;
    PA1 = 1;
    PA2 = 1;
    PA3 = 1;
    PA4 = 1;
    PA5 = 1;
    if (PA3 == 0)
        return 3; //PA3 enable -> key K3
    if (PA4 == 0)
        return 6; //PA4 enable -> key K6
    if (PA5 == 0)
        return 9; //PA5 enable -> key K9

    return 0;
}

void Init_GPIO(void) //normal function
{
    // GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);

    //	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);

    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);

    GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
    PB11 = 1;

    PC12 = 1;
    PC13 = 1;
    PC14 = 1;
    PC15 = 1;
}
