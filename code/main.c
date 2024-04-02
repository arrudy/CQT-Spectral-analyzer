#include "Board_LED.h"
#include "Board_Buttons.h"
#include "LPC17xx.h"
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "Open1768_LCD.h"
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "asciiLib.h"
#include "RTE_Device.h"
#include "RTE_Components.h"
#include "LCD_ILI9325.h"
#include "TP_Open1768.h"
#include "cqt_block.h"

volatile int counter = 0;
volatile int scaler =0;
volatile bool flag_clearscreen=true;

void int_to_string(int,char*,int);
void drawLine(int x_start, int y_start, int width, int height, uint16_t);
void clearScreen(int);

extern ARM_DRIVER_USART Driver_USART1; 
static ARM_DRIVER_USART * USARTdrv = & Driver_USART1;

	void dealey(int decsec){
	for(int i=0;i<decsec*100000;i++);
	}



void EINT0_IRQHandler(void)
{
USARTdrv->Send("\nClick", 7);
	
	
	
LPC_SC->EXTINT |= 1;
}




void ADC_IRQHandler(void){

		
		scaler++;
		if(scaler >= 64){
			int Result = (4095 & (LPC_ADC->ADDR5>>4));
		char a[5] = {' ',' ',' ',' ',' '};
		char b[5] = {'|','|','|','|','|'};
		int_to_string(Result,a,4);
		//USARTdrv->Send("\n", 1);
		USARTdrv->Send(a, 5);
			scaler=0;
		sample_set(counter++,Result);
	/*	for(; counter < WSIZE; ++counter)
{
    sample_set(counter, (int) ((2048. + (1047.f   *sin( 256. * WSIZE / fs * (2.*counter * M_PI / WSIZE) ) ))   )      );
    //fprintf(foutput,"%.2lf ",  (float) (2048 + (int) ((2047*sin(2* 2.*i *  M_PI / WSIZE ))  )));   
}*/

	if(counter >= WSIZE)
	{
			
		counter = 0;
		USARTdrv->Send(b, 5);
		
		float * cqt_result = CQT();
		//clearScreen(20);
		if(flag_clearscreen){ 
			clearScreen(20);
			flag_clearscreen=false;
		}	
		//dealey(1);
		
		
		for(size_t i = 0; i < CQT_RESULT_SIZE; ++i)
		{
			//dealey(1);
		drawLine(20, i * (320/CQT_RESULT_SIZE)    , 240,320/CQT_RESULT_SIZE - 1,LCDWhite);
		drawLine(20, i * (320/CQT_RESULT_SIZE)    , ((int)cqt_result[i])*2,320/CQT_RESULT_SIZE - 1,LCDRed);
		}
		
		
	}
}
	
	int regVal = LPC_ADC->ADSTAT;
	
}
void TIMER0_IRQHandler(void)
{
	USARTdrv->Send(" TTTT ", 5);
	LPC_TIM0->IR = 1; //resetowanie przerwania w peryferium
}
	


	

void drawLine(int y_start, int x_start, int height, int width, uint16_t color)
{
	if(height > 240 - y_start) height =  240 - y_start;
	
	
	for(int j = x_start; j < x_start+width; ++j)
	{
		lcdWriteReg(0x20,y_start);
		lcdWriteReg(0x21,j);
		//lcdWriteReg(0x0022,color);
		lcdWriteIndex(0x0022);
		for(int i = 0 ; i < height; ++i)
		{
			//lcdWriteReg(0x0022,color);
		lcdWriteData(color);
		}
	}
}



void clearScreen(int start_height)
{
	//drawLine(20,20,10,22,LCDWhite);
	
	
	for(int j = 0; j < 320; ++j)
	{
		lcdWriteReg(0x20,start_height);
		lcdWriteReg(0x21,j);
		//lcdWriteReg(0x0022,0xF800);
		//lcdWriteIndex(0x0022);
		for(int i = start_height ; i < 240; ++i)
		{
		//lcdWriteData(LCDWhite);
			lcdWriteReg(0x0022,LCDWhite);
		}
	}
}


void int_to_string(int a, char * tab, int size){
	for(int i = size-1; i >= 0; --i)
	{
	tab[i] = (a%10)+48;
	a /=10;
		if(a==0) break;
	}
}



void drawChar(unsigned char* lit, int x_start, int y_start, uint16_t color){
	for(int j=7;j>=0;j--){
		lcdWriteReg(0x20, x_start);
		lcdWriteReg(0x21, y_start+7-j);
		lcdWriteIndex(0x0022);
			for(int i=0;i<16;i++){
			
			
			if(1<<j & lit[15-i]) lcdWriteData(color);
			else lcdWriteData(0x0000);
			
		}
	}


}


void drawString(const char* list, int y_start, int x_start, uint16_t color){
	int i=0;
	unsigned char buff[16];
	while(list[i]!='\0'){
		GetASCIICode(1,buff,list[i]);
		drawChar(buff,y_start, x_start+i*8, color);
		i++;
	}
}

int main(void){
 
	//USART initialize
   USARTdrv->Initialize(NULL);
	//Power up the USART peripheral /    
	USARTdrv->PowerControl(ARM_POWER_FULL);
	//Configure the USART to 4800 Bits/sec /    
	USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8 | ARM_USART_PARITY_NONE | ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE, 115200);
	// Enable Receiver and Transmitter lines /    
	USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
	USARTdrv->Control (ARM_USART_CONTROL_RX, 1);
	//USARTdrv->Send("\nPress Enter to receive a message", 34);
	//USARTdrv->Send("UART OK", 7);
	{
	char buffer_string[11];
	int buffer = lcdReadReg(OSCIL_ON);
	int_to_string(buffer,buffer_string,10);
	buffer_string[10] = '\t';
	//USARTdrv->Send(buffer_string, 11);
 	}
	
	lcdConfiguration();
	init_ILI9325();
	clearScreen(0);
	drawLine(0,0,20,320,LCDBlack);	
	
	
	char * legend [] = {"32", "64", "128", "256", "512", "1k", "2k", "4k"};
	drawString("Ladowanie wynikow", 110,75, LCDRed);
	
	
	for(uint32_t i = 0; i <  CQT_RESULT_SIZE; i+=bins) drawString(legend[i/bins], 2, i * (320/CQT_RESULT_SIZE), LCDYellow);
	//USARTdrv->Send("LCD  OK", 7);

//Przycisk
	PIN_Configure(2,10, 1, 0,0); //EINT0
	LPC_SC->EXTMODE |= 1;
	LPC_SC->EXTPOLAR = 0;
	LPC_SC->EXTINT = 0;
	NVIC_EnableIRQ(EINT0_IRQn);
	//USARTdrv->Send("ENT0 OK", 7);
	
	
	
//Konfiguracja ADC
	PIN_Configure(1,31, 3, 2, 0); //ADC0.5 pin
	LPC_SC->PCONP |= 1<<12; //ADC power, must be before PDN when enabling
	LPC_ADC->ADCR |= 1<<21; //PDN
	LPC_ADC->ADCR |= 4<<24; //MAT0.1 runs conversion
	LPC_ADC->ADCR |= 1<<5; //Channel 5 of ADC (ADC0.5)
	LPC_ADC->ADINTEN |= 1<<5; //interrupt on Channel 5
	LPC_SC->PCLKSEL0 |= 3<<24; //CCLK/8 dla ADC (13MHz limit)
	//USARTdrv->Send("OK1 ", 5);
	NVIC_EnableIRQ(ADC_IRQn);
	//USARTdrv->Send("ADC  OK", 7);
	
	
	
	
//Konfiguracja MAT0		
//domyslnie wlaczony
	LPC_TIM0->TCR=0; //bit 1: 1 to enable counting, bit 2: 1 to keep timer at zero	
	LPC_SC->PCLKSEL0 |= 1<<2; //CCLK/1 dla MAT0
	PIN_Configure(3,26, 2, 2,0); //MAT0.1

	LPC_TIM0->PR=0; //prescale register
	LPC_TIM0->MR1=3051;	//MR1
	LPC_TIM0->MCR=2<<3;  //bit 1: interrupt, bit 2: reset, bit 3: stop on match. Action on MR1
	LPC_TIM0->EMR |= 1<<1; //EM1: enable for MR1, drive signal into the pin
	LPC_TIM0->EMR |= 3<<6; //EMC1: toggle MAT0.1 on MR1
	NVIC_EnableIRQ(TIMER0_IRQn);
	LPC_TIM0->TCR=1;	//enable timer & stop the halt
	
	
	//USARTdrv->Send("TIM0  OK", 7);
	//USARTdrv->Send("CFG  OK", 7);
	
	//dealey(10);
	//clearScreen(20);
	//dealey(10);
	
	
	//LPC_SC->PCONP|=1;//powerdown 
		SCB->SCR |= 2;
	//LPC_SC->PCONP=0;

	//
			__WFI();
	while(true){

	};
 }
