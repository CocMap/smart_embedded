//------------------------------------------- main.c CODE STARTS ---------------------------------------------------------------------------
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#define DELAYTIME 1000000
#define BUZZER_BIPTIME 5
#define BUZZER_BIPDELAY 2000000
void Delay_s(uint32_t count);
void EINT1_IRQHandler(void);
void Buzzer_bip(uint32_t bip_time);
int main(void)
{
 uint32_t i = 0;
 SYS_Init();
 //LED display via GPIO-C12 to indicate main program execution
 PC->PMD &= ~(0x03)<< 24;
 PC->PMD |= (0x01) << 24;

 //BUZZER to indicate interrupt handling routine
 GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);

 //GPIO Interrupt configuration. GPIO-B15 is the interrupt source
 GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
 PB->IMD &= (~(1ul<<15));			//clear bit 15
 PB->DBEN |= 1ul<<15;					//interrupt enable
 GPIO->DBNCECON |= 1ul<<4;
 PB->IEN |= 1ul<<15;
 NVIC->ISER[0] |= 1ul<<3;

 while(1){
 PC->DOUT ^= 1 << 12;
 Delay_s(DELAYTIME);
 }
}
void EINT1_IRQHandler(void){
 if(PB->ISRC & (1ul<<15)){
 PB->ISRC = PB->ISRC;
 Buzzer_bip(BUZZER_BIPTIME);
 }
 NVIC->ICPR[0] |= 1ul<<3;
 }

void Delay_s(uint32_t count){
 uint32_t n;
 for(n=0;n<count;n++){
 }
 }

void Buzzer_bip(uint32_t bip_time){
 uint32_t i;
 for(i=0;i<(bip_time*2);i++){
 PB11 ^= 1;
 Delay_s(BUZZER_BIPDELAY);
 }
 }
//------------------------------------------- main.c CODE ENDS ---------------------------------------------------------------------------
