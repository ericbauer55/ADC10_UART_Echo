/*
 * init.c
 *
 *  Created on: Dec 19, 2016
 *      Author: Eric
 */

#include "init.h"

void initClocks(){
	/* Basic Clock System Init */
    BCSCTL1 = CALBC1_1MHZ; // Set DCO
    DCOCTL = CALDCO_1MHZ;
    //BCSCTL2 &= ~(DIVS_3); // SMCLK = DCO = 1MHz
}

void initPinIO(){
	/* UART I/O Pins Init */
	P1SEL |= BIT0; // ADC Input Pin P1.0
    P1SEL |= RXD + TXD ; // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= RXD + TXD ; // P1.1 = RXD, P1.2=TXD
}

void initUART(){
	/* UART Module Init */
	UCA0CTL1 |= UCSSEL_2; // SMCLK

    UCA0BR0 = 0x08; // 1MHz 115200
    UCA0BR1 = 0x00; // 1MHz 115200

    UCA0MCTL = UCBRS2 + UCBRS0; // Modulation UCBRSx = 5
    UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**

    //UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    UC0IE |= UCA0TXIE; // Enable USCI_A0 TX interrupt
}

void initADC10(){
	/* ADC10 Init: Single Channel, Single Conversion */
	ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // sampling window, ADC10ON, interrupt enabled
	ADC10CTL1 = INCH_0;                       // input A0 (Pin P1.0)
	ADC10AE0 |= BIT0;                         // PA.0 ADC option select
}



