/*********************************************************
 * Pre-processor Directives
 *********************************************************/
#include <msp430.h> 
#include "msp430g2553.h"
#include "init.h"
#include <stdint.h>
#define TXD BIT2
#define RXD BIT1

/*********************************************************
 * Global Variables
 *********************************************************/
typedef enum{
	SendNull,
	SendA,
	SendB,
}UARTStateEnum;
uint8_t adcPacket[2]; // UART 8-bit packet structure for one sample
UARTStateEnum UARTState; // UART packet state machine counter for sending one sample
int sampleTxDone; // flag is true if the ADC sample has been transmitted, thus S&C a new value;


/*********************************************************
 * Main Function
 *********************************************************/
int main(void)
{
   WDTCTL = WDTPW + WDTHOLD; // Stop WDT
   /* Initialize Modules */
   initClocks();
   initPinIO();
   initUART();
   initADC10();
   initTimer();
   /* Initialize Variables */
   sampleTxDone = 1;
   adcPacket[0] = 0; adcPacket[1] = 0;
   /* Main Loop */
   while (1)
   {
	   if(sampleTxDone){
		   // Start a new conversion if the previous sample has been TX'd
		   ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		   __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
		   // Fit converted sample in packets (S=UART start bit, P=UART stop bit, X = ADC10MEM data bit)
		   adcPacket[1] = (ADC10MEM<<1) & (0x7E); // least significant bits (sent 1st) ::S0XXXXXX0P
		   adcPacket[2] = ((ADC10MEM>>2) & (0xF0)) | 0x01; // most significant bits (sent 2nd) :: SXXXX0001P
	   }
	   __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit

   }
}

/*********************************************************
 * Interrupt Service Routines
 *********************************************************/
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{

}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{

}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
	switch(UARTState){ // state checked after Timer ISR exits
	   case SendNull:
		   // do nothing with UART, NULL Packet
		   sampleTxDone = 0; // flag for TX in progress
		   UARTState = SendA;
		   break;
	   case SendA:
		   // Begin UART TX process
		   // SendA: send lsb data and post-increment the packet index
		   UCA0TXBUF = adcPacket[0];
		   UARTState = SendB;
		   break;
	   case SendB:
		   // SendB: send msb data, post-increment packet index to reset state machine after next while(busy){} loop
		   UCA0TXBUF = adcPacket[1];
		   // End UART TX process
		   UARTState = SendNull;
		   sampleTxDone = 1;
		   break;
	}
	__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}
