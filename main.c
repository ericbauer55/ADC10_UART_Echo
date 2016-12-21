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
uint8_t adcPacket[3]; // UART 8-bit packet structure for one sample
uint8_t packetCounter; // UART packet state machine counter for sending one sample
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
   /* Initialize Variables */
   sampleTxDone = 1;
   adcPacket[0] = 0; adcPacket[1] = 0; adcPacket[2] = 0;
   packetCounter = 0;
   /* Main Loop */
   while (1)
   {
	   if(sampleTxDone){
		   // Start a new conversion if the previous sample has been TX'd
		   ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		   // Fit converted sample in packets (S=UART start bit, P=UART stop bit, X = ADC10MEM data bit)
		   adcPacket[1] = (ADC10MEM<<1) & (0x7E); // least significant bits (sent 1st) ::S0XXXXXX0P
		   adcPacket[2] = ((ADC10MEM>>2) & (0xF0)) | 0x01; // most significant bits (sent 2nd) :: SXXXX0001P
	   }
	   __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
	   switch(packetCounter){ // state checked after Timer ISR exits
	   case 0:
		   // do nothing with UART
		   UC0IE &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
		   sampleTxDone = 0; // flag for TX in progress
		   packetCounter++;
		   break;
	   case 1:
		   // Begin UART TX process
		   UC0IE |= UCA0TXIE; // Enable USCI_A0 TX interrupt
		   // packetCounter = 1: send lsb data and post-increment the packet index
		   UCA0TXBUF = adcPacket[packetCounter++];
		   break;
	   case 2:
		   // packetCounter = 2: send msb data, post-increment packet index to reset state machine after next while(busy){} loop
		   UCA0TXBUF = adcPacket[packetCounter];
		   // End UART TX process
		   packetCounter = 0;
		   sampleTxDone = 1;
		   break;
	   }

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
	__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}
