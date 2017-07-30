/*
 * NRF24L01.cpp
 *
 * Created: 7/25/2017 4:53:19 PM
 *  Author: orencollaco
 */ 

#include "NRF24L01.h"

//#define DEBUG_UART
//#define STATS

uint8_t init_NRF_TX(uint8_t data){
	writeSPIRegister(0x11,32,1);
	if(data){
		ADC_Init(admux_c,adcsra_c);
		ADC_Start();
		writeSPIRegister(0x00,0x0A,1); //Power ON and Transmit mode
		if(readSPIRegister(0x00,1) != 0x0A)
			return 0x00;
		else
			return 0x01;
	}
	else{
		init_T1_PWM();
		writeSPIRegister(0x00,0x0B,1);//Power ON and Receive mode
		if(readSPIRegister(0x00,1) != 0x0B)
		return 0x00;
		else
		return 0x01;
	}
}

void printInfo(){
	#ifdef UART_SUPPORTED
	printStringCRNL(" FIFO Status : ");
	printHexNumber(readSPIRegister(0x17,1),1);
	printString(" Status : ");
	printHexNumber(sendSPICommand(0xFF),1);
	printString(" TX Observe : ");
	printHexNumber(readSPIRegister(0x08,1),1);
	//printHexNumber(millis(),8);
	#endif
}

uint8_t isTXFull(){
	return ((uint8_t)readSPIRegister(0x17,1)& 0x20);
}

uint8_t isRXFull(){
	return ((uint8_t)readSPIRegister(0x17,1)& 0x02);
}


uint8_t isTXEmpty(){
	return ((uint8_t)readSPIRegister(0x17,1) & 0x10);
}

uint8_t isRXEmpty(){
	return ((uint8_t)readSPIRegister(0x17,1) & 0x01);
}

uint8_t isTX_DS_Set(){
	return ((uint8_t)readSPIRegister(0x07,1) & 0x20);
}

void transmitPayload(volatile uint8_t *data){
	static uint8_t status, fstatus, recount = 0;
	static uint16_t pcount = 0, dcount = 0;
	static bool packetInFIFO = 0, linklost = 1;
	#ifdef STATS
	static uint64_t t;
	if(millis() - t > 1000){
		t = millis();
		
		printStringCRNL("Speed = ");
		printHexNumber(pcount,2);
		printString(" Drops = ");
		printHexNumber(dcount,2);
		dcount = 0;
		pcount = 0;
	}
	#endif
	if(linklost){
		
		status = sendSPICommand(0xFF);
		fstatus = (uint8_t)readSPIRegister(0x17,1);
		if(!(fstatus & 0x20)){
			#ifdef DEBUG_UART
			Notify(PSTR("Establishing link..."));
			#endif
			writeToFIFO(data);
		}
		if(status & 0x20){
			#ifdef UART_SUPPORTED 
			Notify(PSTR("Link established! Connected."));
			#endif 
			writeSPIRegister(0x07,0x20,1);
			linklost = 0;
		}
		if(status & 0x01)
			sendSPICommand(0xE1);
		if(status & 0x10)
			writeSPIRegister(0x07,0x10,1);
	}
	if(!linklost){
		status = sendSPICommand(0xFF);
		if((status & 0x20)){
			#ifdef DEBUG_UART
			Notify(PSTR("Packet Transmitted"));
			#endif
			pcount += 1;
			writeSPIRegister(0x07,0x20,1);
		}
		if(!(status & 0x01)){
			#ifdef DEBUG_UART
			Notify(PSTR("Transmitting packet..."));
			#endif
			writeToFIFO(data);
			return;
		}
		if(status & 0x10){
			#ifdef DEBUG_UART
			Notify(PSTR("Packet Transmission failed"));
			#endif
			dcount += 1;	
			if(recount < 20){
				#ifdef DEBUG_UART
				Notify("Retransmitting...");
				#endif
				writeSPIRegister(0x07,0x10,1);
				recount += 1;
			}
			else{
				#ifdef UART_SUPPORTED
				Notify(PSTR("Maximum retransmit reached. TX FIFO Flushed. Link lost"));
				#endif
				recount = 0;
				sendSPICommand(0xE1);
				packetInFIFO = 0;
				linklost = 1;
			}
		}
		if(status & 0x01){
			
			#ifdef DEBUG_UART
			Notify(PSTR("TX Buffer Full, Waiting..."));
			#endif
		}
	}
}

void fastTransferPayload(volatile uint8_t data){
	static uint8_t status;
	status = sendSPICommand(0xFF);
	if(!(status & 0x01)){
		PORTB &= ~(1 << 0);
		SPI_MasterTransmit(0xA0);
		SPI_MasterTransmit(data);
		PORTB |= (1 << 0);
		return;
	}
	PORTB |= (1 << 0);
	if(status & 0x10)
	writeSPIRegister(0x07,0x10,1);
}

void writeToFIFO(volatile uint8_t *data){
	static uint8_t i;
	PORTB &= ~(1 << 0);
	SPI_MasterTransmit(0xA0);
	for(i = 0; i < 32; i++){
		SPI_MasterTransmit(*data);
		data += 1;
	}
	PORTB |= (1 << 0);
}

void readFIFO(volatile uint8_t *data){
	PORTB &= ~(1 << 0);
	SPI_MasterTransmit(0x61);
	for(uint8_t i = 0; i < 32; i++){
		*data =  SPI_MasterTransmit(0x00);
		data += 1;
	}
	PORTB |= (1 << 0);
} 

uint8_t readFIFObyte(){
	PORTB &= ~(1 << 0);
	SPI_MasterTransmit(0x61);
	return SPI_MasterTransmit(0x00);
	PORTB |= (1 << 0);
}

void writeFIFObyte(volatile uint8_t data){
	PORTB &= ~(1 << 0);
	SPI_MasterTransmit(0xA0);
	SPI_MasterTransmit(data);
	PORTB |= (1 << 0);
}

uint8_t sendSPICommand(uint8_t cmd){
	uint8_t data;
	PORTB &= ~(1 << 0);
	data = SPI_MasterTransmit(cmd);
	PORTB |= (1 << 0);
	return	data;
}

uint64_t readSPIRegister(uint8_t addr, uint8_t length){
	uint64_t data = 0;
	PORTB &= ~(1 << 0);
	SPI_MasterTransmit(addr);
	for(uint8_t i = 0; i < length; i++){
	data |= SPI_MasterTransmit(0x00);
	data = data << 8;
	}
	data >>= 8;
	PORTB |= (1 << 0);
	return	data;
}

uint8_t writeSPIRegister(uint8_t addr, uint64_t data, uint8_t length){
	uint8_t status;
	PORTB &= ~(1 << 0);
	addr |= 0x20;
	status = SPI_MasterTransmit(addr);
	for(uint8_t i = 0; i < length; i++){
		SPI_MasterTransmit(data);
		data = data >> 8;
	}
	PORTB |= (1 << 0);
	return status;
}