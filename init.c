/*
 * init.c
 *
 *  Created on: Dec 19, 2016
 *      Author: Eric
 */

#include "init.h"

void initClocks(){
	/* Basic Clock System Init */
    BCSCTL1 = CALBC1_16MHZ; // Set DCO
    DCOCTL = CALDCO_16MHZ; // SMCLK = DCO = 1MHz
}

void initPinIO(){
	/* UART I/O Pins Init */
	P1SEL |= BIT0; // ADC Input Pin P1.0
    P1SEL |= RXD + TXD ; // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= RXD + TXD ; // P1.1 = RXD, P1.2=TXD
    /* NULL Packet UART Pin Settings */
    P1DIR |= TXD; // set TXD as an output
    P1OUT |= TXD; // set TXD as a high setting for NULL packet
}

void initUART(){
	/* UART Module Init */
	UCA0CTL1 |= UCSSEL_2; // SMCLK

    UCA0BR0 = 0x02; // 16MHz 500000 baud
    UCA0BR1 = 0x00; // 16MHz 500000 baud

    UCA0MCTL = UCOS16; // Modulation UCBRSx = 0, UCBRFx = 0
    UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**

    //UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    //UC0IE |= UCA0TXIE; // Enable USCI_A0 TX interrupt
}

void initADC10(){
	/* ADC10 Init: Single Channel, Single Conversion */
	ADC10CTL0 = ADC10SHT_1 + ADC10ON + ADC10IE; // sampling window, ADC10ON, interrupt enabled
	ADC10CTL1 = INCH_0;                       // input A0 (Pin P1.0)
	ADC10AE0 |= BIT0;                         // PA.0 ADC option select
}



