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
    System_Config();

    //Uyen code
    //LED display via GPIO-C12 to indicate main program execution
    PC->PMD = (PC->PMD & ~(0x3 << (12 << 1))) | (0x1 << (12 << 1)); // Set GPC12 to output mode

    PB->PMD = (PB->PMD & ~(0x3 << (13 << 1))) | (0x1 << (13 << 1)); // Set GPB13 to output mode
                                                                    //GPIO Interrupt configuration. GPIO-B15 is the interrupt source
                                                                    // GPB15 = SW_INT1
    PB->PMD = (PB->PMD & ~(0x3 << (15 << 1))) | (0x0 << (15 << 1));

    PB->IMD &= (~(1ul << 15));  // Set up edge triggered interrupt at IMD[n] (n = 15)
                                // Because we are setting up GPB15 as the interrupt source
                                // => Corresponding IsnipMD bit is also n = 15
    PB->IEN |= 1ul << 15;       // Set interrupt enable to be triggered at falling clock edge
    PB->DBEN |= 1ul << 15;      // De-bounce enable. The reason for (<<15) is the same as IMD
    GPIO->DBNCECON |= 1ul << 4; // Set De-bounce counter clock source is the internal 10 kHz LIRC
    NVIC->ISER[0] |= 1ul << 3;  //Set PB15 interrupt ISENA (Vector 20 - Bit4 = IRQ4)

    //--------------------------------
    //LCD initialization
    //--------------------------------

    SPI3_Config();

    LCD_start();
    LCD_clear();

    // Start Loop
    while (1)
    {

        PC->DOUT ^= 1 << 12;
        Delay_us(DELAYTIME);
        Dist = &distance;
        PB->DOUT &= ~(1 << 13);
        PB->DOUT &= ~(1 << 15);
        Delay_us(2);
        PB->DOUT |= (1 << 13);
        Delay_us(10);
        PB->DOUT &= ~(1 << 13);

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
    long duration = 0;
    if (PB->ISRC & (1ul << 15))
    { // Read if interrupt is generated
        duration = counter();
        PB->ISRC = PB->ISRC; // Clear the correspond pending interrupt
    }
    *Dist = duration * 0.034 / 2;
    NVIC->ICPR[0] |= 1ul << 3; // Remove pending state of associated interrupt
}

void Delay_us(uint32_t count)
{
    uint32_t n;
    for (n = 0; n < (count / 3); n++)
    {
    }
}
long counter()
{
    long count = 0;
    while (PB->PIN & 1ul << 15)
        count++;
    return count / 3;
}

void System_Config(void)
{
    SYS_UnlockReg();        // Unlock write protected register
    CLK->PWRCON |= HXT_CLK; // 4~24MHz = 12MHz High speed External Clock Source
    while (!(CLK->CLKSTATUS & HXT_STATUS))
        ;                   // Check Clock status whether it is stable
    CLK->PWRCON |= LXT_CLK; // 32kHz Low speed External Clock Source
    while (!(CLK->CLKSTATUS & LXT_STATUS))
        ;                             // Check Clock status whether it is stable
    CLK->PLLCON &= ~(1ul << 18);      // 0 = PLL FOUT Enabled
    CLK->PLLCON &= ~(1ul << 16);      // 0 = PLL is in Normal mode
    CLK->PLLCON &= PLLCON_FB_DV_MASK; // Clear FB_DV
    CLK->PLLCON |= PLLCON_FB_DV_VAL;  // Set to a value for calculating FOUT, FOUT = (FB_DV_VAL + 2)
    while (!(CLK->CLKSTATUS & PLL_STATUS))
        ;                         // Check PLL Clock status whether it is stable
    CLK->CLKSEL0 &= CLK_SEL_MASK; //Clear Clock select setting
    CLK->CLKSEL0 |= PLL_CLK_SEL;  // Select PLL Clock

    CLK->CLKDIV &= ~0x0F;                // Erase all CPU clock divider
    CLK->CLKDIV |= (CPUCLOCKDIVIDE - 1); // Set CPU clock with no division
                                         // CLK->CLKSEL1 &= ~(7ul << 20); // Clear Clock selection of timer 3
                                         // CLK->CLKSEL1 |= 2ul << 20; // Timer 3 clock HCLK selected
                                         // CLK->APBCLK |= 1ul << 5;		// TIMER 3 selected
    SYS_LockReg();                       // Lock System write protected register
                                         //--------------------------------
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
