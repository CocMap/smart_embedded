void HCSR04_GPIO_Init(void);
void HCSR04_Trigger(void);
void Delay_us(uint32_t count);

void TIMER2_Init(void);
void TIMER2_Start(void);
void TIMER2_Stop(void);
void EINT0_IRQHandler(void);
void EINT1_IRQHandler(void);
void Interrupt_Init (void);

volatile long duration = 0;
volatile uint32_t distance = 0;
volatile uint8_t Capture = 0;

//----------------------------HCSR04 FUNCTION----------------------------------------------//

//HCSR04 configure
void HCSR04_GPIO_Init(void)
{
    PB->PMD = (PB->PMD & ~(0x3 << (4 << 1))) | (0x1 << (4 << 1));   // Output
    PB->PMD = (PB->PMD & ~(0x3 << (15 << 1))) | (0x0 << (15 << 1)); // Input
    PB->PMD = (PB->PMD & ~(0x3 << (14 << 1))) | (0x0 << (14 << 1)); // Input

    PB->DOUT &= ~(1 << 4);
}

//HCSR04 trigger
void HCSR04_Trigger(void)
{
    //Trigger of Ultrasonic Sensor
    PB->DOUT |= 1 << 4;
    Delay_us(10); // Delay       // 10us for TRIG width
    PB->DOUT &= ~(1 << 4);
}

//----------------------------TIMER2 FUNCTION----------------------------------------------//
//TIMER2 INITILIZATION
void TIMER2_Init(void)
{
    TIMER2->TCSR &= ~(3ul << 27);
    TIMER2->TCSR |= (3ul << 27); // Continuous

    TIMER2->TCSR |= (0x31 << 0); // Output 1MHz

    /* Step 4. Disable interrupt */
    TIMER2->TCSR &= ~(1 << 29);
    TIMER2->TISR |= 1ul << 0; //Write 1 to clear for safety

    TIMER2->TCSR &= ~(1 << 30); //Disable Timer2

    TIMER2->TCSR |= 1 << 16; // Enable TDR function
}

//TIMRE2 START
void TIMER2_Start(void)
{
    TIMER2->TCSR |= (0x31 << 0); // TIME2 frequency = 50/(49+1) = 1Mhz
    TIMER2->TCSR |= (1 << 30);   //Start Counting Timer2
}

//TIMER2 STOP
void TIMER2_Stop(void)
{
    TIMER2->TCSR |= 1ul << 26;  //Reset TIMER2
    TIMER2->TCSR &= ~(1 << 30); //Disable Timer2
}

//-----------------------------------Interrupt-----------------------------------//
//TIMER2 INITIALIZATION
void Interrupt_Init(void)
{
    //GPIO Interrupt configuration. GPIO-B15 is the interrupt source
    // GPB15 = SW_INT1
    // Input - Output are set
    PB->IMD &= (~(1ul << 14));
    PB->IMD &= (~(1ul << 15)); // Set up edge triggered interrupt at IMD[n] (n = 15)
                               // Because we are setting up GPB15 as the interrupt source
                               // => Corresponding IsnipMD bit is also n = 15
    PB->IEN |= 1ul << 30;      //Pin 14 rising edge
    PB->IEN |= 1ul << 15;      // Set interrupt enable to be triggered at falling clock edge
    NVIC->ISER[0] |= 1ul << 2;
    NVIC->ISER[0] |= 1ul << 3; //Set PB15 interrupt ISENA (Vector 20 - Bit4 = IRQ4)
}

//EINT0 INTERRUPT
void EINT0_IRQHandler(void)
{ // Function to initialise EINT0 interrupt on GPB14
    if (PB->ISRC & (1ul << 14))
    { // Read if interrupt is generated
        TIMER2_Start();
        PB->ISRC = PB->ISRC; // Clear the correspond pending interrupt
    }
    NVIC->ICPR[0] |= 1ul << 2; // Remove pending state of associated interrupt
}

//EINT1 INTERRUPT
void EINT1_IRQHandler(void)
{ // Function to initialise EINT1 interrupt on GPB15
    if (PB->ISRC & (1ul << 15))
    {                           // Read if interrupt is generated
        duration = TIMER2->TDR; // Output signal to buzzer when interrupt happens
        TIMER2_Stop();
        Capture = 1;
        PB->ISRC = PB->ISRC; // Clear the correspond pending interrupt
    }
    NVIC->ICPR[0] |= 1ul << 3; // Remove pending state of associated interrupt
}

//----------------------------------Delay_us--------------------------------------//
void Delay_us(uint32_t count)
{
    uint32_t n;
    for (n = 0; n < (count / 3); n++)
    {
    }
}
