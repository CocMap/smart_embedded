//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     :
//Author      :	Nha Tran
//Date        : 
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : Turn on/off LED (GPIO Port C pin 12~15)
//
//
//-------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------
//Peripherals initialization. Following are the most common tasks:
//1. Enable peripheral clock
//2. Configure I/O pins.
//3. Configure the peripheral (e.g. operating mode, etc.)
//4. Configure peripheral interrupt (in case peripheral interrupt is used in your application)
//-------------------------------------------------------------------------------------------------------------	


#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

int main(void)
{
    SYS_Init();		//system initialization
	
		//set mode for GPIO	
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);			//configure GPIO Port C/12 as GPIO_MODE_OUTPUT
		GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
		GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
		GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	
		//main program - standard driver
    while(1){
			PC12 = 0; 										//turn on LED
			CLK_SysTickDelay(100000);			//delay 1000000us = 0.1s
			PC12 = 1;											//turn off LED
			
			PC13 = 0; 										//turn on LED
			CLK_SysTickDelay(100000);			//delay 1000000us = 0.1s
			PC13 = 1;											//turn off LED
			
			PC14 = 0; 										//turn on LED
			CLK_SysTickDelay(100000);			//delay 1000000us = 0.1s
			PC14 = 1;											//turn off LED
			
			PC15 = 0; 										//turn on LED
			CLK_SysTickDelay(100000);			//delay 1000000us = 0.1s
			PC15 = 1;											//turn off LED
			}
}
