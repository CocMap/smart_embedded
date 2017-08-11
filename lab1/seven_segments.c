//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     :
//Author      :	Nha Tran
//Date        : 
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : 7-segments 
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

int number = 0;
int digit = 0;
int row = 0;
			int col = 0;

int main(void)
{
    SYS_Init();		//system initialization
		//int number=0;
	
		//change to PMD - pin mode control
		//7-segments: GPE0~7 (row) and GPC4~7 (column)
	
		PE->PMD &= ~(0xFFFF);							//clear 16 first bits (0~15)
		PE->PMD |= (0x5555);							//set 16 bits as 01010101 -> set output mode
	
		PC->PMD &= ~(0x0F) << 4;					//clear 4 bits 4~7
		PC->PMD |= (0x5) << 5;						//ship 101 << 5 (bit 4~7 = 0101)				
		
		//main program 
    while(1){
			
			//using PORT C to choose the digit (column)
			//using PORT E to write the number (7-segments in each digit)
			
			//write 9 number 
			switch(number){
				case 0:
					number = (0x11);
					break;
				case 1:	
					number =  (0x11);
					break;
				case 2:
					number =  (0x3C);
					break;
				case 3:
					number = (0xB9);
					break;
				case 4:
					number = (0x85);
					break;
				case 5:
					number = (0xAD);
					break;
				case 6:
					number = (0xED);
					break;
				case 7:
					number = (0x19);
					break;
				case 8:
					number = (0xFD);
					break;
				case 9:
					number = (0xBD);
					break;
				default:
					number = (0x02);
					break;
			}
			
			switch(digit){
			case 2:
				digit = (0x20);
				break;
			case 3:
				digit = (0x40);
				break;
			case 4:
				digit = (0x80);
				break;
			default:
				digit = (0x10);
				break;
		}
			
			
		`	for (col=0; col < 5;col++) {
				PC->PMD ^= digit;
				for (row=0; row < 9; row++){
					PE->DOUT ^= number;	
					CLK_SysTickDelay(100000);
			}
			
		
		}
}
