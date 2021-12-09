#define F_CPU 4000000UL        /* using default clock 4MHz*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "UART_1_AVR128DA64.h"
#include <stdbool.h>
#define channel_3 0x03
#define channel_4 0x04
#define START_TOKEN 0x03
#define END_TOKEN 0xFC

unsigned long size=0, SYRINGE_Value=0;

void ADC0_init(void)
{
	VREF.ADC0REF = 0x5 | (1 << 7);
	ADC0.CTRLC = ADC_PRESC_DIV4_gc;        // CLK_PER divided by 4
	ADC0.CTRLA = ADC_ENABLE_bm;
	PORTD.IN  = ADC_RESSEL_12BIT_gc;
}


void ADC0_start(void)
{
	
	ADC0.COMMAND = ADC_STCONV_bm;
}

float ADC0_read(int pin)
{
	
	ADC0.MUXPOS = pin;
	ADC0_start();
	
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
	return ADC0.RES;
}

struct syringe 	
{
	bool syringe_detection;
    float syringe_dia;
		
}s1;
	
int main(void)
	 { 
		sei();
		s1.syringe_detection = true;   
		s1.syringe_dia;
		USART1_init(9600);
		ADC0_init();
		float adc ;
		PORTC.DIRCLR = PIN7_bm; //output PORTC PIN7
		PORTC.PIN7CTRL |= PORT_PULLUPEN_bm;
		
		while (1)
		{
			if (!(PORTC.IN & (PIN7_bm)))
			{
				
				USART1_sendString("SYRINGE DETECTED");
				
			}
			else
			{
				USART1_sendString("SYRINGE NOT DETECTED");
			}
			float adc= ADC0_read( channel_3);
			float sum=0.00;
			for(int i=1;i<=500;i++)
			{
				adc= ADC0_read(channel_3);  //ADC value read
				sum=sum+adc;
			}
			sum=sum/500.00;
			USART1_sendFloat(sum,2);
	        s1.syringe_dia = 0.02140996*(sum) - 4.123888;  //60ml,20ml,10ml,5ml syringe diameter //
	        USART1_sendFloat(s1.syringe_dia, 2);
	       _delay_ms(100);
		}
	 }
