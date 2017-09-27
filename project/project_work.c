#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"


//------------------------------------------------------------------------------------------------------------------------------------
// Definitions
//------------------------------------------------------------------------------------------------------------------------------------

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
#define DELAYTIME 1000000 // 1M unit

//-----------------------------------------------------//
void System_Config(void);

void HCSR04_GPIO_Init(void);
void HCSR04_Trigger(void);
void Delay_us(uint32_t count);

void TIMER2_Init(void);
void TIMER2_Start(void);
void TIMER2_Stop(void);
void EINT0_IRQHandler(void);
void EINT1_IRQHandler(void);
void Interrupt_Init (void);

//----------------------------------------------------//
void SPI3_Config(void);

void LCD_start(void);
void LCD_command(unsigned char temp);
void LCD_data(unsigned char temp);
void LCD_clear(void);
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr);

//--------------------------------------------------//

volatile long duration = 0;
volatile uint32_t distance = 0;
volatile uint8_t Capture = 0;

char display[1024];

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
	
		
    while (1) {
			
			HCSR04_Trigger();
			if (Capture == 1) {
				LCD_clear();
				//distance = (0.034*duration)/(2);
				distance = duration/58;																	//......................Oh well, datasheet... :|
				snprintf(display, sizeof(display), "Distance: %d", distance);
				printS_5x7(10, 8, display);
				Capture = 0;
			}
			CLK_SysTickDelay(60000); // Delay
    }
}

//------------------------------------------------------------------------------------------------------------------------------------
// Functions definition
//------------------------------------------------------------------------------------------------------------------------------------
void System_Config(void)
{
    SYS_UnlockReg(); // Unlock protected registers
    // Enable clock sources
    //HXT - External High frequency Crystal 12 MHz
    CLK->PWRCON |= HXT_CLK;
    while (!(CLK->CLKSTATUS & HXT_STATUS));
    //LXT - External Low frequency Crystal 32 kHz
    CLK->PWRCON |= LXT_CLK;
    while (!(CLK->CLKSTATUS & LXT_STATUS));

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
    CLK->CLKDIV |= (CPUCLOCKDIVIDE - 1); // No Prescaling

		//Timer 2 initialise
		CLK->CLKSEL1 &= ~(7ul << 16); //Clear clock selection for TIMER2
		CLK->CLKSEL1 |= (0x2 << 16); //Choose HCLK clock for TIMER2
		CLK->APBCLK |= 1ul << 4; // TMR2_EN
		SYS->GPB_MFP |= 1ul << 10;             //GPIOB Multiple Function and Input Type Control Register 
                                                    //set 01 for bit 10 -> GPB10 become TMR2
    SYS->ALT_MFP &= ~(1ul << 0);           //Alternative Multiple Function Pin Control Register 
		
    //enable clock of SPI3
    CLK->APBCLK |= 1ul << 15;

    SYS_LockReg(); // Lock protected registers
}

void HCSR04_GPIO_Init(void)
{
    PB->PMD = (PB->PMD & ~(0x3 << (4 << 1))) | (0x1 << (4 << 1)); // Output
		PB->PMD = (PB->PMD & ~(0x3 << (15 << 1))) | (0x0 << (15 << 1));	// Input
		PB->PMD = (PB->PMD & ~(0x3 << (14 << 1))) | (0x0 << (14 << 1)); // Input

    PB->DOUT &= ~(1 << 4);
}

/*
    Trigger HCSR04
*/
void HCSR04_Trigger(void)
{
    //Trigger of Ultrasonic Sensor
    PB->DOUT |= 1 << 4;
    CLK_SysTickDelay(10); // Delay       // 10us for TRIG width
    PB->DOUT &= ~(1 << 4);
}

//----------------------------TIMER----------------------------------------------//
void TIMER2_Init(void)
{
    TIMER2->TCSR &= ~(3ul << 27);
		TIMER2->TCSR |= (3ul << 27); // Continuous

    TIMER2->TCSR |= (0x31 << 0);    		 // Output 1MHz
                               
    /* Step 4. Disable interrupt */
    TIMER2->TCSR &= ~(1 << 29);
    TIMER2->TISR |= 1ul << 0;        //Write 1 to clear for safety       

    TIMER2->TCSR &= ~(1 << 30);        //Disable Timer2

		TIMER2->TCSR |=1 << 16;        // Enable TDR function
}
void TIMER2_Start(void)
{       
    TIMER2->TCSR |= (0x31 << 0);    // TIME2 frequency = 50/(49+1) = 1Mhz
    TIMER2->TCSR |= (1 << 30);        //Start Counting Timer2
   
}

void TIMER2_Stop(void)
{
    TIMER2->TCSR |= 1ul << 26;        //Reset TIMER2   
    TIMER2->TCSR &= ~(1 << 30);        //Disable Timer2
}


//-----------------------------------Interrupt-----------------------------------//
void Interrupt_Init (void) {
//GPIO Interrupt configuration. GPIO-B15 is the interrupt source
																					// GPB15 = SW_INT1
// Input - Output are set
	PB->IMD &= (~(1ul<<14));
	PB->IMD &= (~(1ul<<15)); 								// Set up edge triggered interrupt at IMD[n] (n = 15)
																					// Because we are setting up GPB15 as the interrupt source
																					// => Corresponding IsnipMD bit is also n = 15
	PB->IEN |= 1ul<<30;												//Pin 14 rising edge
	PB->IEN |= 1ul<<15; 											// Set interrupt enable to be triggered at falling clock edge
	NVIC->ISER[0] |= 1ul<<2;
	NVIC->ISER[0] |= 1ul<<3;									//Set PB15 interrupt ISENA (Vector 20 - Bit4 = IRQ4)
}

void EINT0_IRQHandler(void){			// Function to initialise EINT0 interrupt on GPB14
	if(PB->ISRC & (1ul<<14)){				// Read if interrupt is generated
		TIMER2_Start ();
		PB->ISRC = PB->ISRC;					// Clear the correspond pending interrupt
	}
	NVIC->ICPR[0] |= 1ul<<2;				// Remove pending state of associated interrupt
}

void EINT1_IRQHandler(void){			// Function to initialise EINT1 interrupt on GPB15
	if(PB->ISRC & (1ul<<15)){				// Read if interrupt is generated
		duration = TIMER2->TDR;		// Output signal to buzzer when interrupt happens
		TIMER2_Stop ();
		Capture = 1;
		PB->ISRC = PB->ISRC;					// Clear the correspond pending interrupt
	}
	NVIC->ICPR[0] |= 1ul<<3;				// Remove pending state of associated interrupt
 }
 
//----------------------------------Delay_us--------------------------------------//
void Delay_us(uint32_t count){
	uint32_t n;
	for(n=0;n<(count/3);n++){
		}
 }

//----------------------------------LCD Display ----------------------------------//
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


