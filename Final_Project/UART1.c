/**
 * @file UART1.c
 *
 * @brief Source code for the UART1 driver.
 *
 * This file contains the function definitions for the UART1 driver.
 *
 * @note For more information regarding the UART module, refer to the
 * Universal Asynchronous Receivers / Transmitters (UARTs) section
 * of the TM4C123GH6PM Microcontroller Datasheet.
 * Link: https://www.ti.com/lit/gpn/TM4C123GH6PM
 *
 * @note Assumes that the system clock (50 MHz) is used.
 *
 * @author Aaron Nanas
 */

#include "UART1.h"

void UART1_Init(void)
{
    // Enable the clock to UART1 by setting the 
    // R1 bit (Bit 1) in the RCGCUART register
    SYSCTL->RCGCUART |= 0x02;
    
    // Enable the clock to Port C by setting the
    // R2 bit (Bit 2) in the RCGCGPIO register
    SYSCTL->RCGCGPIO |= 0x04;
    
    // Disable the UART1 module before configuration by clearing
    // the UARTEN bit (Bit 0) in the CTL register
    UART1->CTL &= ~0x01;
    
    // Set the baud rate by writing to the DIVINT field (Bits 15 to 0)
    // and the DIVFRAC field (Bits 15 to 0) in the IBRD and FBRD registers, respectively.
    UART1->IBRD = 325; // Integer part
    UART1->FBRD = 33;  // Fractional part
    
    // Configure the data word length to 8 bits
    UART1->LCRH |= 0x60;
    
    // Enable the transmit and receive FIFOs
    UART1->LCRH |= 0x10;
    
    // Select one stop bit
    UART1->LCRH &= ~0x08;
    
    // Disable the parity bit
    UART1->LCRH &= ~0x02;
    
    // Enable the UART1 module
    UART1->CTL |= 0x01;
    
    // Configure the C5 (U1RX) and C7 (U1TX) pins to use the alternate function
    GPIOC->AFSEL |= 0xA0; // Enable alternate functions for PC5 and PC7
    
    // Clear the PMC7 and PMC5 fields in the PCTL register before configuration
    GPIOC->PCTL &= ~0xF00F0000;
    
    // Configure the C5 pin as U1RX and C7 pin as U1TX
    GPIOC->PCTL |= 0x20002000;
    
    // Enable the digital functionality for the C5 and C7 pins
    GPIOC->DEN |= 0xA0;
}


char UART1_Input_Character(void)
{
	while((UART1->FR & UART1_RECEIVE_FIFO_EMPTY_BIT_MASK) != 0);
	
	return (char)(UART1->DR & 0xFF);
}

void UART1_Output_Character(char data)
{
	while((UART1->FR & UART1_TRANSMIT_FIFO_FULL_BIT_MASK) != 0);
	UART1->DR = data;
}

uint32_t UART1_Input_String(char *buffer_pointer, uint16_t buffer_size) 
{
	int length = 0;
	uint32_t string_size = 0;
	
	// Read the last received data from the UART Receive Buffer
	char character = UART1_Input_Character();
	
	// Check if the received character is a carriage return. Otherwise,
	// for each valid character, increment the string_size variable which will
	// indicate how many characters have been detected from the input string
	while(character != UART1_CR)
	{
		// Remove the character from the buffer is the received character is a backspace character
		if (character == UART1_BS)
		{
			if (length)
			{
				buffer_pointer--;
				length--;
				UART1_Output_Character(UART1_BS);
			}
		}
		
		// Otherwise, if there are more characters to be read, store them in the buffer
		else if (length < buffer_size)
		{
			*buffer_pointer = character;
			buffer_pointer++;
			length++;
			string_size++;
		}
		character = UART1_Input_Character();
	}
	*buffer_pointer = 0;
	
	return string_size;
}

void UART1_Output_String(char *pt)
{
	while(*pt)
	{
		UART1_Output_Character(*pt);
		pt++;
	}
}