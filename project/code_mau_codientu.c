/*
    Van toc song am truyen trong khong khi v ~= 343m/s
    Cong thuc tinh van toc: d = v*t/2 (m)
    Sau khi ban tin hieu tu chan Trigger, chan Echo se dua tin hieu len muc cao,
        - neu co tin hieu phan hoi, chan Echo se dua xuong thap, do rong xung phan hoi tu 100uS -> 18ms
        - neu khong do duoc vat thi sau khoang 36ms chan Echo se tu dong dua xuong muc thap
  
    Khoang thoi gian tu khi nhan xung phan hoi den lan ban tin hieu Trigger tiep theo >=10ms
*/

/*
    TIMER2 CAPTURE dung de bat tin hieu suon chan Echo cua SRF04
    TIMER2 de do do rong xung
        - Tan so TIMER2 f = 1Mhz -> T = 1us
        - Do rong xung phan hoi tu 100uS - 36us --> Gia tri thanh ghi TIMR2: Echo_Width = 100 -> 36000
            d = v*t/2 = (343*10^3)*(Echo_Width*10^6)/2 (mm)
            d = (343*Echo_Width)/(2*10^3)
*/

#include <stdio.h>
#include "NUC1xx.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvUART.h"
#include "Driver\DrvSYS.h"

#define _SR04A_ECHO (GPB_2) //NUC140VE3xN, Pin34/PB2/T2EX
#define _SR04A_TRIG (GPB_4) //NUC140VE3xN, Pin19/PB4/RXD1
#define _SR04A_TRIG_Low (GPB_4 = 0)
#define _SR04A_TRIG_High (GPB_4 = 1)

void TIMER2_Init(void);
void SRF04_GPIO_Init(void);
void SRF04_Trigger(void);
void SRF04_Distance_Display(uint32_t distance); // hien thi LCD

volatile uint32_t SRF04_Echo_Width = 0;
volatile uint32_t distance = 0;
volatile uint32_t SRF04_Echo_Flag = FALSE;

//--------------------------------- TMR2 Interrupt Handler
/* TEXCON.RSTCAPSEL
= 1: reset counter
= 0: capture mode select and tranfer value form Timer counter to TCAP when TEXIF = 1
*/

void TMR2_IRQHandler(void)
{
    TIMER2->TEXCON.RSTCAPSEL = 0; // set back for falling edge to capture
    TIMER2->TCSR.CEN = 1;         //Timer Start

    if (TIMER2->TEXISR.TEXIF == 1) //Capture Flag (TEXIF)
    {
        TIMER2->u32TEXISR |= 0x01;       //Clear Capture Flag (TEXIF)
        SRF04_Echo_Width = TIMER2->TCAP; //Load Capture Value (Unit: us)
        SRF04_Echo_Flag = TRUE;
    }
}

//---------------------------------------------------------------------------------TIMER2
// Timer Clock:    12 MHz
// Prescale:        11
// Compare:        0xffffff
// Mode:            One-Shot mode
// Capture:        Enable, Capture with Falling Edge
void TIMER2_Init(void)
{
    //Step 1: T2EX pin Enable (PB.2, Pin34)
    SYS->GPBMFP.UART0_nRTS_nWRL = 1;
    SYS->ALTMFP.PB2_T2EX = 1;

    //Step 2: Timer Controller Reset and Setting Clock Source
    SYS->IPRSTC2.TMR2_RST = 1;  //Timer Controller: Reset
    SYS->IPRSTC2.TMR2_RST = 0;  //Timer Controller: Normal
    SYSCLK->CLKSEL1.TMR2_S = 0; //Timer Clock = 12 MHz
    SYSCLK->APBCLK.TMR2_EN = 1; //Timer Clock Enable

    //Step 3: Timer Controller Setting
    //  TMR0_T = (12 MHz / (11+1) / 1000000)^-1 = 1.000 Second
    TIMER2->TCMPR = 0xffffff;   //Timer Compare Value:  [0~16777215]
    TIMER2->TCSR.PRESCALE = 11; //Timer Prescale:       [0~255]
    TIMER2->TCSR.MODE = 0;      //Timer Operation Mode: One-Shot

    //Step 4: External Capture Mode Setting
    TIMER2->TEXCON.TEXEN = 1;     //External Capture Function Enable
    TIMER2->TEXCON.RSTCAPSEL = 0; //Capture Mode Select: Capture Mode
    TIMER2->TEXCON.TEX_EDGE = 2;  //Capture Edge: Rising & Falling

    //Step 5: Timer Interrupt Setting
    //    TIMER2->TCSR.IE = 1;                      //Timeout Interrupt Enable
    //    TIMER2->u32TISR |= 0x01;              //Clear Timeout Flag (TIF)
    TIMER2->TEXCON.TEXIEN = 1; //Capture Interrupt Enable
                               //    TIMER2->u32TEXISR |= 0x01;              //Clear Capture Flag (TEXIF)
    TIMER2->TEXISR.TEXIF = 1;  //Clear Capture Flag (TEXIF)
    NVIC_EnableIRQ(TMR2_IRQn); //Timer NVIC IRQ Enable

    //Step 6: Start Timer Capture (Set by Ultrasonic_Trigger() Function)
    //     TIMER2->TCSR.CRST = 1;                  //Timer Counter Reset
    //     TIMER2->TCSR.CEN = 1;                      //Timer Start
}

//---------------------------------------------------------------------------------SRF04
void SRF04_GPIO_Init(void)
{
    //        //Ultrasonic I/O Pins Initial
    //    GPIOB->PMD.PMD2 = 0;                            //_SR04_ECHO pin, Input
    //    GPIOB->PMD.PMD4 = 1;              //_SR04_TRIG pin, Output
    //  _SR04A_TRIG_Low;                  // set Trig output to Low

    DrvGPIO_Open(E_GPB, 2, E_IO_INPUT);
    DrvGPIO_Open(E_GPB, 4, E_IO_OUTPUT);
    _SR04A_TRIG_Low;
}
/*
    Trigger SRF04 and Reset Timer Counter
*/
void SRF04_Trigger(void)
{
    //Trigger of Ultrasonic Sensor
    _SR04A_TRIG_High;
    DrvSYS_Delay(10); // 10us for TRIG width
    _SR04A_TRIG_Low;

    TIMER2->TEXCON.RSTCAPSEL = 1; // set for rising edge trigger to reset counter
}

//---------------------------------------------------------------------------------main
int main(void)
{
    char TEXT1[16] = "D =     mm";
    STR_UART_T sParam; // khai báo bien structure UART

    // DrvSYS_UnlockProtectedReg(); // unlock register for programming

    // DrvSYS_SetOscCtrl(E_SYS_XTL12M, ENABLE);
    // while (DrvSYS_GetChipClockSourceStatus(E_SYS_XTL12M) == 0)
    //     ;
    // DrvSYS_Open(32000000); // set System Clock to run at 32MHz
    //                        // 12MHz crystal input, PLL output 32MHz

    // DrvSYS_LockProtectedReg();

    /* Thiet lap UART cho chân RX0/GPB0 và TX0/PB1 */
    DrvGPIO_InitFunction(E_FUNC_UART0);

    /* Thiet dat các thông so UART*/
    sParam.u32BaudRate = 9600;
    sParam.u8cDataBits = DRVUART_DATABITS_8;
    sParam.u8cStopBits = DRVUART_STOPBITS_1;
    sParam.u8cParity = DRVUART_PARITY_NONE;
    sParam.u8cRxTriggerLevel = DRVUART_FIFO_1BYTES;

    /* Khoi tao UART0 voi các thông so thiet dat cua bien sParam */
    DrvUART_Open(UART_PORT0, &sParam);

    printf("\n\n");
    printf("+-------------------------------------------------------------------+\n");
    printf("|                    SRF04 TIMER CAPTURE Sample Code                 \n");
    printf("+-------------------------------------------------------------------+\n");

    SRF04_GPIO_Init();
    TIMER2_Init();

    while (1)
    {
        SRF04_Trigger();
        DrvSYS_Delay(40000);
        if (SRF04_Echo_Flag == TRUE)
        {
            SRF04_Echo_Flag = FALSE;
            distance = (340 * SRF04_Echo_Width) / (2000);
            sprintf(TEXT1 + 6, " %d mm  \n", distance);
            printf(TEXT1);
            SRF04_Echo_Flag = 0;
        }
        DrvSYS_Delay(50000);
        DrvSYS_Delay(50000);
    }
}