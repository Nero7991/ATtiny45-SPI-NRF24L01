/*
 * ATtiny45.cpp
 *
 * Created: 7/29/2017 6:15:54 PM
 *  Author: orencollaco
 */ 


#include "ATtiny45.h"

volatile uint64_t time;

void setup(){
	SPI_MasterInit();
	
}

void SPI_MasterInit(){
	
}

uint8_t SPI_MasterTransmit(uint8_t data){
	register uint8_t a,b;
	a = (1 << USIWM0) | (1 << USITC);
	b = (1 << USIWM0) | (1 << USITC) | (1 << USICLK);
	USIDR = data;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	return USIDR;
	
}

void Timer0_Init(unsigned char tccr0a,unsigned char tccr0b){
	TCCR0A = tccr0a;
	TCCR0B = tccr0b;
}

void init_T1_PWM(){
	//TCCR1 = tccr1;
	//GTCCR = gtccr;
	PLLCSR = 0x06;
}
void ADC_Init(unsigned char admux, unsigned char adcsra)
{
	ADCSRA = adcsra;
	ADMUX = admux;
}
void ADC_ChangeChannelTo(unsigned char admux)
{
	admux&=0x0F;
	admux|=admux_c;
	ADMUX = admux;
}

unsigned char ADC_Read(unsigned char admux)
{
	admux&=0x0F;
	admux|=admux_c;
	ADMUX = admux;
	while(!(ADCSRA & (1<<ADIF)))
	;
	PORTB ^= (1<<5);
	return ADCH;
}
void ADC_Start()
{
	ADCSRA = adcsra_c | (1<<ADSC);
}

uint64_t millis(){
	return time;
}