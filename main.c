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
uint8_t adcPacket[2]; // UART 8-bit packet structure for one sample
uint8_t packetCounter; // UART packet state machine counter for sending one sample
int sampleTxDone; // flag is true if the ADC sample has been transmitted, thus S&C a new value;
int x[3]; // sample input buffer

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
   adcPacket[0] = 0; adcPacket[1] = 0;
   packetCounter = 0;
   x[0] = 0; x[1] = 0; x[2] = 0;
   /* Main Loop */
   while (1)
   {
	   /* Sampling and Filtering */
		ADC10CTL0 |= ENC + ADC10SC;		// Sampling and conversion start
		while(ADC10CTL1 & ADC10BUSY);	// Wait for the sample to be done
		x[2] = x[1]; x[1] = x[0]; x[0] = ADC10MEM; // Rotate the sample buffer
		/* Sample UART Transmit */
		if(sampleTxDone){
			// If UART is not currently sending a 10-bit sample, packetize new y[0] value
			// Fit converted sample in packets (S=UART start bit, P=UART stop bit, X = ADC10MEM data bit)
			adcPacket[0] = (x[0]<<1) & (0x7E); // least significant bits (sent 1st) ::S0XXXXXX0P
			adcPacket[1] = ((x[0]>>2) & (0xF0)) | 0x01; // most significant bits (sent 2nd) :: SXXXX0001P
		}
		if(!(UCA0STAT & UCBUSY)){ // See if UART is not busy sending a sample
			if(packetCounter <= 1){
				sampleTxDone = 0; // reset sample TX flag during TX process
				UCA0TXBUF = adcPacket[packetCounter++]; // send lsb data and post-increment the packet index
			} else {
				packetCounter = 0;
			    sampleTxDone = 1;
			}
		}
   }
}

/*********************************************************
 * Interrupt Service Routines
 *********************************************************/
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	/*
	// Fit converted sample in packets (S=UART start bit, P=UART stop bit, X = ADC10MEM data bit)
	adcPacket[0] = (ADC10MEM<<1) & (0x7E); // least significant bits (sent 1st) ::S0XXXXXX0P
	adcPacket[1] = ((ADC10MEM>>2) & (0xF0)) | 0x01; // most significant bits (sent 2nd) :: SXXXX0001P
	// Begin UART TX process
	UC0IE |= UCA0TXIE; // Enable USCI_A0 TX interrupt
	UCA0TXBUF = adcPacket[packetCounter++]; // send lsb data and post-increment the packet index
	sampleTxDone = 0; // reset sample TX flag during TX process
	*/
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
	/*
  while (!(IFG2 & UCA0TXIFG)); // USCI_A0 TX buffer ready?
  if(packetCounter == 1){ // transmit msb data only once
	  // send msb data, post-increment packet index to reset state machine after next while(busy){} loop
	  UCA0TXBUF = adcPacket[packetCounter++];
  } else { // reset state machine
	  packetCounter = 0;
	  sampleTxDone = 1;
	  //UC0IE &= ~UCA0TXIE; // Enable USCI_A0 TX interrupt
	  //__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
  } */
}


