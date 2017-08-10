//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     :
//Author      :	Nha Tran
//Date        : 
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : 
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

//int sec = 1000000;

int main(void)
{
    SYS_Init();		//system initialization
	
		//change to PMD - pin mode control
		//modify pin12~15 -> modify PMD12~PMD15 -> bit24~31 (Technical Manual)
	
		PC->PMD &= ~(0xFF) << 24;				//clear 8 first bits (24~31)
		PC->PMD |= (0x55) << 24;				//set 8 bits as 01010101 -> set output mode
		
		//main program 
    while(1){
			int i = 0;
			
			for (i = 12; i < 16; i++) {
			
				PC->DOUT ^= 1<<i;		//flip bit (0 to 1, 1 to 0)
				CLK_SysTickDelay(100000);
			}
		}
}
