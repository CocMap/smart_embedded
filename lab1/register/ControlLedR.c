//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     :
//Author      :
//Date        : 
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   :
//
//
//-------------------------------------------------------------------------------------------------------------
// GPIO_LED : GPIO output to control an on-board red LED
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// low-active output control by GPC12

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

int main(void)
{
 //-------------------------------------------------------------------------------------------------------------
 //System initialization
 //-------------------------------------------------------------------------------------------------------------
    SYS_Init();
 
 //-------------------------------------------------------------------------------------------------------------
 //Peripherals initialization. Following are the most common tasks:
 //1. Enable peripheral clock
 //2. Configure I/O pins.
 //3. Configure the peripheral (e.g. operating mode, etc.)
 //4. Configure peripheral interrupt (in case peripheral interrupt is used in your application)
 //------------------------------------------------------------------------------------------------------------- 

	//change to PMD (pin mode control)
  PC->PMD &= ~(0xFF)<< 24;			//clear 8 first bits (bit 31 - bit 24)
  PC->PMD |= (0x01) << 25;			//set bit 24 to 1

  PC->PMD |= (0x01) << 27;			//set bit 26 to 1
	
  PC->PMD |= (0x01) << 29;			//set bit 28 to 1
	
  PC->PMD |= (0x01) << 31;			//set bit 30 to 1
	
//	PB->PMD &= ~(0x03)<< 22;
//	PB->PMD |= (0x01) << 22;
 //-------------------------------------------------------------------------------------------------------------
 //Main program
 //-------------------------------------------------------------------------------------------------------------
    while(1){
			int i;
			for (i = 12; i < 16; i++) {
				PC->DOUT ^= 1 << i;
				CLK_SysTickDelay(500000); // Delay
				PC->DOUT ^= 1 << i;
			}
		}
}
