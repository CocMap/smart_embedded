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
#define PLLCON_FB_DV_VAL 1
#define CPUCLOCKDIVIDE 1
#define DELAYTIME 1000000 // 1M unit

uint8_t *Dist = NULL;
uint8_t distance = 0;
void Buzzer_bip(uint32_t bip_time);
void System_Config(void);
void SPI3_Config(void);

void LCD_start(void);
void LCD_command(unsigned char temp);
void LCD_data(unsigned char temp);
void LCD_clear(void);
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr);

void KeyPadEnable(void);
uint8_t KeyPadScanning(void);

void Delay_us(uint32_t count);
void EINT1_IRQHandler(void);
long counter();

int main(void)
{
    uint8_t pressKey = 0;
    char display[1024];
    //--------------------------------
    //System initialization
    //--------------------------------
 
    //Uyen code
    //LED display via GPIO-C12 to indicate main program execution
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

   PB->PMD |= (0x01) << 22; //configure output for led8
    //--------------------------------
    //LCD initialization
    //--------------------------------
   System_Config();

    SPI3_Config();

    LCD_start();
    LCD_clear();

    // Start Loop
    while (1)
    {

        PC->DOUT ^= 1 << 12;
        Delay_us(100000);
        // Delay_us(DELAYTIME);
        // Dist = &distance;
        // PB->DOUT &= ~(1 << 13);
        // PB->DOUT &= ~(1 << 15);
        // Delay_us(2);
        // PB->DOUT |= (1 << 13);
        // Delay_us(10);
        // PB->DOUT &= ~(1 << 13);

        while (pressKey == 0)
            pressKey = KeyPadScanning(); //scan for any press activity
        switch (pressKey)
        {
        //*****anh edit k? t? đo?n này nha
        case 1: //1 = key number 1
            LCD_clear();
            snprintf(display, sizeof(display), "Distance: %d", *Dist);
            printS_5x7(10, 8, display);
            CLK_SysTickDelay(200000);

            //printC(row[0-7], col[0-127], ascii_code)

            break;

        case 2:
            LCD_clear();
            printS_5x7(10, 8, "Hello Anh Uyên cu x? teeeee: ");
            break;

        default:
            break;
        }
        CLK_SysTickDelay(200000); //to avoid bouncing keypress
        pressKey = 0;
    }
}

void EINT1_IRQHandler(void)
{ // Function to initialise EINT1 interrupt on GPB15
    //long duration = 0;
    if (PB->ISRC & (1ul << 15))
    { // Read if interrupt is generated
        //duration = counter();
        PB->ISRC = PB->ISRC; // Clear the correspond pending interrupt
        Buzzer_bip(5); //buzzer bip 5 times
    }
    //*Dist = duration * 0.034 / 2;
    NVIC->ICPR[0] |= 1ul << 3; // Remove pending state of associated interrupt
}

void Buzzer_bip(uint32_t bip_time)
{
    uint32_t i;
    for (i = 0; i < (bip_time * 2); i++)
    {
        PC->DOUT ^= 1 << 15;
        PB11 ^= 1;
        Delay_us(200000); //Buzzer bip deplay time
        //PB->DOUT ^= 0 << 11;
    }
}

void Delay_us(uint32_t count)
{
    uint32_t n;
    for (n = 0; n < (count / 3); n++)
    {
    }
}
// long counter()
// {
//     long count = 0;
//     while (PB->PIN & 1ul << 15)
//         count++;
//     return count / 3;
// }

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
    while (!(CLK->CLKSTATUS & PLL_STATUS))
        ;

    //clock source selection
    CLK->CLKSEL0 &= CLK_SEL_MASK;
    CLK->CLKSEL0 |= PLL_CLK_SEL;

    //clock division
    CLK->CLKDIV &= ~0x0F;
    CLK->CLKDIV |= (CPUCLOCKDIVIDE - 1);

    //enable clock of SPI3
    CLK->APBCLK |= 1ul << 15;

    SYS_LockReg(); // Lock protected registers
}

void SPI3_Config(void)
{
    SYS->GPD_MFP |= 1ul << 11; //1: PD11 is configured for alternative function
    SYS->GPD_MFP |= 1ul << 9;  //1: PD9 is configured for alternative function
    SYS->GPD_MFP |= 1ul << 8;  //1: PD8 is configured for alternative function

    SPI3->CNTRL &= ~(1ul << 23); //0: disable variable clock feature
    SPI3->CNTRL &= ~(1ul << 22); //0: disable two bits transfer mode
    SPI3->CNTRL &= ~(1ul << 18); //0: select Master mode
    SPI3->CNTRL &= ~(1ul << 17); //0: disable SPI interrupt
    SPI3->CNTRL |= 1ul << 11;    //1: SPI clock idle high
    SPI3->CNTRL &= ~(1ul << 10); //0: MSB is sent first
    SPI3->CNTRL &= ~(3ul << 8);  //00: one transmit/receive word will be executed in one data transfer

    SPI3->CNTRL &= ~(31ul << 3); //Transmit/Receive bit length
    SPI3->CNTRL |= 9ul << 3;     //9: 9 bits transmitted/received per data transfer

    SPI3->CNTRL |= (1ul << 2); //1: Transmit at negative edge of SPI CLK
    SPI3->DIVIDER = 0;         // SPI clock divider. SPI clock = HCLK / ((DIVIDER+1)*2). HCLK = 50 MHz
}

//------------------------------------------------------------------------------------------------------------------------------------
// Functions definition
//------------------------------------------------------------------------------------------------------------------------------------
void LCD_start(void)
{
    LCD_command(0xE2); // Set system reset
    LCD_command(0xA1); // Set Frame rate 100 fps
    LCD_command(0xEB); // Set LCD bias ratio E8~EB for 6~9 (min~max)
    LCD_command(0x81); // Set V BIAS potentiometer
    LCD_command(0xA0); // Set V BIAS potentiometer: A0 ()
    LCD_command(0xC0);
    LCD_command(0xAF); // Set Display Enable
}
void LCD_command(unsigned char temp)
{
    SPI3->SSR |= 1ul << 0;
    SPI3->TX[0] = temp;
    SPI3->CNTRL |= 1ul << 0;
    while (SPI3->CNTRL & (1ul << 0))
        ;
    SPI3->SSR &= ~(1ul << 0);
}
void LCD_data(unsigned char temp)
{
    SPI3->SSR |= 1ul << 0;
    SPI3->TX[0] = 0x0100 + temp;
    SPI3->CNTRL |= 1ul << 0;
    while (SPI3->CNTRL & (1ul << 0))
        ;
    SPI3->SSR &= ~(1ul << 0);
}
void LCD_clear(void)
{
    int16_t i;
    LCD_SetAddress(0x0, 0x0);

    for (i = 0; i < 132 * 8; i++)
    {
        LCD_data(0x00);
    }
}
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr)
{
    LCD_command(0xB0 | PageAddr);
    LCD_command(0x10 | (ColumnAddr >> 4) & 0xF);

    LCD_command(0x00 | (ColumnAddr & 0xF));
}
void KeyPadEnable(void)
{
    GPIO_SetMode(PA, BIT0, GPIO_MODE_QUASI);
    GPIO_SetMode(PA, BIT1, GPIO_MODE_QUASI);
    GPIO_SetMode(PA, BIT2, GPIO_MODE_QUASI);
    GPIO_SetMode(PA, BIT3, GPIO_MODE_QUASI);
    GPIO_SetMode(PA, BIT4, GPIO_MODE_QUASI);
    GPIO_SetMode(PA, BIT5, GPIO_MODE_QUASI);
}
uint8_t KeyPadScanning(void)
{
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
