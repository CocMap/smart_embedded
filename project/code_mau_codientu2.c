/*
    Van toc song am truyen trong khong khi v ~= 343m/s
    Cong thuc tinh van toc: d = v*t/2 (m)
    Sau khi ban tin hieu tu chan Trigger, chan Echo se dua tin hieu len muc cao,
        - neu co tin hieu phan hoi, chan Echo se dua xuong thap, do rong xung phan hoi tu 100uS -> 18ms
        - neu khong do duoc vat thi sau khoang 36ms chan Echo se tu dong dua xuong muc thap
  
    Khoang thoi gian tu khi nhan xung phan hoi den lan ban tin hieu Trigger tiep theo >=10ms
*/

/*
    INT1 dung de bat tin hieu suon chan Echo cua SRF04
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

#define    _SR04A_ECHO           (GPB_15)            //NUC140VE3xN, Pin91/PB.15/INT1
#define    _SR04A_TRIG           (GPB_4)            //NUC140VE3xN, Pin19/PB.4/RXD1
#define    _SR04A_TRIG_Low     (GPB_4 = 0)
#define    _SR04A_TRIG_High (GPB_4 = 1)

#define rising 1
#define falling 2
#define both 3

void TIMER2_Start(void);
void TIMER2_Stop(void);
void TIMER2_Init(void);

void SRF04_GPIO_Init(void);
void SRF04_Trigger(void);
void SRF04_Distance_Display(uint32_t distance); // hien thi LCD

char TEXT1[16] = "D =     mm";

volatile uint32_t SRF04_Echo_Width = 0;
volatile uint32_t distance = 0;
volatile uint32_t  INT1_Trigger;
volatile uint32_t SRF04_Echo_Flag  = FALSE;


External Interrupt Handler (INT button to trigger GPB14)
void EINT1Callback(void)
{
    if(INT1_Trigger == rising)
    {
        TIMER2_Start();
        DrvGPIO_DisableEINT1();
        DrvGPIO_EnableEINT1(E_IO_FALLING, E_MODE_EDGE, EINT1Callback); // cau hinh ngat suon xuong
        INT1_Trigger = falling;
    }
    else
    {
        SRF04_Echo_Width = TIMER2->TDR;   
        TIMER2_Stop();
        DrvGPIO_DisableEINT1();
        DrvGPIO_EnableEINT1(E_IO_RISING, E_MODE_EDGE, EINT1Callback); // cau hinh ngat suon len
        INT1_Trigger = rising;
        SRF04_Echo_Flag = 1;       
    }
}

int main (void)
{

    STR_UART_T sParam; // khai báo bien structure UART

    DrvSYS_UnlockProtectedReg(); // unlock register for programming   
   
    DrvSYS_SetOscCtrl(E_SYS_XTL12M, ENABLE);
   DrvSYS_Open(32000000);// set System Clock to run at 32MHz
                         // 12MHz crystal input, PLL output 32MHz
   
    DrvSYS_LockProtectedReg();
   
    /* Thiet lap UART cho chân RX0/GPB0 và TX0/PB1 */
    DrvGPIO_InitFunction(E_FUNC_UART0);

    /* Thiet dat các thông so UART*/
    sParam.u32BaudRate         = 9600;
    sParam.u8cDataBits         = DRVUART_DATABITS_8;
    sParam.u8cStopBits         = DRVUART_STOPBITS_1;
    sParam.u8cParity         = DRVUART_PARITY_NONE;
    sParam.u8cRxTriggerLevel= DRVUART_FIFO_1BYTES;

    /* Khoi tao UART0 voi các thông so thiet dat cua bien sParam */
     DrvUART_Open(UART_PORT0,&sParam);

    printf("\n\n");
    printf("+-------------------------------------------------------------------+\n");
    printf("|                      SRF04 EXT-INT Sample Code                      |\n");
    printf("+-------------------------------------------------------------------+\n");
    
    SRF04_GPIO_Init();
    TIMER2_Init();
    DrvGPIO_SetDebounceTime(8, E_DBCLKSRC_HCLK);    

   while(1)
    {
       
        SRF04_Trigger();
        DrvSYS_Delay(40000); 
        if(SRF04_Echo_Flag == 1)
        {
            distance = (340*SRF04_Echo_Width)/(2000);
            sprintf(TEXT1+6, " %d mm  \n", distance);
            printf(TEXT1) ;
            SRF04_Echo_Flag = 0;
            DrvSYS_Delay(50000);
            DrvSYS_Delay(50000);
        }
       
    }
}

//---------------------------------------------------------------------------------TIMER2
// Timer Clock:    12 MHz
// Prescale:        11
// Compare:        0xffffff
// Mode:            One-Shot mode
void TIMER2_Init(void)
{
    SYSCLK->CLKSEL1.TMR2_S = 0;    //Select 12Mhz for Timer2 clock source
  SYSCLK->APBCLK.TMR2_EN =1;    //Enable Timer2 clock source
    TIMER2->TCSR.MODE = 0;        //Select periodic mode for Periodic mode

    TIMER2->TCSR.PRESCALE=11;    // TIME2 frequency = 12/(11+1) = 1Mhz
    TIMER2->TCMPR = 16777215;        // Set TCMPR is 24 bits register [0~16777215]
                               
    /* Step 4. Disable interrupt */
    TIMER2->TCSR.IE = 0;
    TIMER2->TISR.TIF = 1;        //Write 1 to clear for safty       

    TIMER2->TCSR.CEN = 0;        //Disable Timer2

//      TIMER2->TCSR.TDR_EN=1;        // Enable TDR function
}

void TIMER2_Start(void)
{       
    TIMER2->TCSR.PRESCALE=11;    // TIME2 frequency = 12/(11+1) = 1Mhz
    TIMER2->TCMPR = 16777215;        // Set TCMPR is 24 bits register [0~16777215]
    TIMER2->TCSR.CEN = 1;        //Start Counting Timer2
   
}

void TIMER2_Stop(void)
{
    TIMER2->TCSR.CRST = 1;        //Reset TIMER2   
    TIMER2->TCSR.CEN = 0;        //Disable Timer2
}

//---------------------------------------------------------------------------------SRF04
/*
    Khoi tao huong chan dieu khien
*/
void SRF04_GPIO_Init(void)
{
    // DrvGPIO_Open(E_GPB, 4, E_IO_OUTPUT);
    // DrvGPIO_Open(E_GPB, 15, E_IO_INPUT);
   
    // cau hinh chan ngat ngoaiINT1, ngat suon len
    DrvGPIO_InitFunction(E_FUNC_EXTINT1);
    DrvGPIO_EnableEINT1(E_IO_RISING, E_MODE_EDGE, EINT1Callback); // configure external rising interrupt
    INT1_Trigger = rising;
    SRF04_Echo_Flag = 0;

    _SR04A_TRIG_Low;
}

// /*
//     Trigger SRF04
// */
void SRF04_Trigger(void)
{
    //Trigger of Ultrasonic Sensor
    _SR04A_TRIG_High;
    DrvSYS_Delay(10);                            // 10us for TRIG width
    _SR04A_TRIG_Low;
}
