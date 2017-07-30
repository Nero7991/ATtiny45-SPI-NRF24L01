/*
 * ATtiny45-SPI-NRF24L01.cpp
 *
 * Created: 7/29/2017 4:48:10 PM
 * Author : orencollaco
 */ 

#include <avr/io.h>
volatile uint8_t NewPayload,buf1[32],buf2[32],buf1full;
#include "NRF24L01.h"
int main(void)
{
	uint8_t PRIM_RX = 1;
	if(!init_NRF_TX(!PRIM_RX))
	SPI_MasterTransmit(0);
		while(1);
		
	while (1)
	{
		if(!PRIM_RX){
			if(NewPayload){
				NewPayload = 0;
				if(buf1full)
				transmitPayload(buf1);
				else
				transmitPayload(buf2);
			}
		}
		if(PRIM_RX){
			if(!isRXEmpty()){
				if(buf1full)
				readFIFO(buf2);
				else
				readFIFO(buf1);
				buf1full ^= 1;
				//printStringCRNL(hexToString(buf1));
				}
				else{
					//printStringCRNL("Nothing to read, Waiting...");
				}
			}
		
    }
}

