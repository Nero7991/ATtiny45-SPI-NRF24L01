/*
 * ATtiny45.h
 *
 * Created: 7/29/2017 6:16:22 PM
 *  Author: orencollaco
 */ 


#ifndef ATTINY45_H_
#define ATTINY45_H_

#define F_CPU 16000000UL
//#define UART_SUPPORTED
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <inttypes.h>

#define adcsra_c 0xA2
#define admux_c 0x20

void ADC_Init( unsigned char, unsigned char);
void ADC_ChangeChannelTo(unsigned char admux);
unsigned char ADC_Read(unsigned char );
void ADC_Start();
void setup();
void Timer0_Init(unsigned char tccr0a,unsigned char tccr0b);
void init_T1_PWM();
uint8_t sendSPICommand(uint8_t cmd);
uint8_t SPI_MasterTransmit(uint8_t data);
uint64_t millis();
void SPI_MasterInit();

#endif /* ATTINY45_H_ */