#include <stdio.h>

#define    _SR04A_ECHO           (GPB_15)            //NUC140VE3xN, Pin91/PB.15/INT1
#define    _SR04A_TRIG           (GPB_4)            //NUC140VE3xN, Pin19/PB.4/RXD1
#define    _SR04A_TRIG_Low     (GPB_4 = 0)
#define    _SR04A_TRIG_High (GPB_4 = 1)

// #define rising 1
// #define falling 2
// #define both 3

//void SRF04_GPIO_Init(void);
void Interrup_Config(void);
void Edge_Falling_Enable(void); 
void Edge_Rising_Enable(void);
void Set_Debounce(void);

void TIMER2_Start(void);
void TIMER2_Stop(void);
void TIMER2_Init(void);

void SRF04_Trigger(void);


volatile uint32_t SRF04_Echo_Width = 0;
volatile uint32_t distance = 0;
volatile uint32_t INT1_Trigger;
volatile uint32_t SRF04_Echo_Flag  = FALSE;

//---------------------------------------------------------------------------------SRF04
void Interrup_Config(void){
    //--------------------CONFIGURE INTERRUPT--------------------
    //Step 1 - Choose the Interrupt pin -> configure input (PB15)
    PB->PMD &= ~(0x03) << 30; //clear current 24~25 bit
    PB->PMD |= (0x00) << 30;  //configure 00 as input

    //Step 2 - Set LEVEL or EDGE trigger interrupt
    NVIC->ISER[0] |= (1 << 3); //ENABLE EXTERNAL interrupt

}

void Edge_Rising_Enable(void) {
    PB->IMD &= ~(1 << 15);      //IMD = 0 -> EDGE interrupt for PB15
                                //if the interrupt is EDGE trigger, the trigger source can be
                                //controlled by de-bounce 
    PB->DBEN |= (1<<15);        //enable de-bounce 
    GPIO->DBNCECON |= 1 << 4;    //de-bounce counter clock source - internal 10kHz                        

    PB->IEN |= (0 << 15);      //detect RISING EDGE
                                //enable PB15
    NVIC->ISER[0] |= (1 << 3); //ENABLE EXTERNAL interrupt
                                
}

void Edge_Falling_Enable(void){
    PB->IMD &= ~(1 << 15);      //IMD = 0 -> EDGE interrupt for PB15
    //if the interrupt is EDGE trigger, the trigger source can be
    //controlled by de-bounce 
    PB->DBEN |= (1<<15);        //enable de-bounce 
    GPIO->DBNCECON |= 1 << 4;    //de-bounce counter clock source - internal 10kHz                        

    PB->IEN |= (1 << 15);      //detect FALLING EDGE
    //enable PB15
    NVIC->ISER[0] |= (1 << 3); //ENABLE EXTERNAL interrupt
    
}

void Interrupt_Disable(void){
    NVIC->ISER[0] |= (0 << 3); //ENABLE EXTERNAL interrupt    
}

void SRF04_GPIO_Init(void)
{
    // DrvGPIO_Open(E_GPB, 4, E_IO_OUTPUT);
    // DrvGPIO_Open(E_GPB, 15, E_IO_INPUT);

    //GPB2 echo -> input
    PB->PMD &= ~(3ul << 30);         //clear 4~5 -> input
     
    //GPB4 trigger -> output
    PB->PMD &= ~(3ul << 8);         //clear 8~9
    PB->PMD |= (1ul << 8);          //output

    
    
    //---------------------
   
    // cau hinh chan ngat ngoaiINT1, ngat suon len
    //DrvGPIO_InitFunction(E_FUNC_EXTINT1);                          //configure I/O for INT1
    //DrvGPIO_EnableEINT1(E_IO_RISING, E_MODE_EDGE, EINT1Callback); // configure external rising interrupt and enable
    Interrup_Config();                          //configure I/O for INT1
    Edge_Rising_Enable();                       // configure external rising interrupt and enable
    EINT1Callback();
    
    INT1_Trigger = rising;
    SRF04_Echo_Flag = 0;

    //_SR04A_TRIG_Low;
    //sensor trigger LOW
    PB->DOUT &= ~(1ul << 4);         //clear = 0 -> set LOW 
}

void EINT1Callback(void)
{
    if(INT1_Trigger == rising)
    {
        TIMER2_Start();
        Interrupt_Disable();
        Edge_Falling_Enable();  // cau hinh ngat suon xuong
        EINT1Callback();
        INT1_Trigger = falling;
    }
    else
    {
        SRF04_Echo_Width = TIMER2->TDR;   
        TIMER2_Stop();
        Interrupt_Disable();
        Edge_Rising_Enable(); // cau hinh ngat suon len
        EINT1Callback();
        INT1_Trigger = rising;
        SRF04_Echo_Flag = 1;       
    }
}
/*
    Trigger SRF04 and Reset Timer Counter
*/
void SRF04_Trigger(void)
{
    //Trigger of Ultrasonic Sensor
    PB->DOUT |= (1ul << 4);            //_SR04A_TRIG_High;
    CLK_SysTickDelay(10);               // 10us for TRIG width
    PB->DOUT &= ~(1ul << 4);            //_SR04A_TRIG_Low;     //clear = 0 -> set LOW  

    TIMER2->TEXCON |= (1ul << 4);       //TIMER2->TEXCON.RSTCAPSEL = 1;         // set for rising edge trigger to reset counter
}

//---------------------------------------------------------------------------------main
void Set_Debounce(void) {
    PB->DBEN |= (1<<15);        //enable de-bounce 
    GPIO->DBNCECON |= 1 << 4;    //de-bounce counter clock source - internal 10kHz                      
}