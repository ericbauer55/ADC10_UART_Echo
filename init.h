/*
 * init.h
 *
 *  Created on: Dec 19, 2016
 *      Author: Eric Bauer
 */
#include "msp430g2553.h"
#define TXD BIT2
#define RXD BIT1

#ifndef INIT_H_
#define INIT_H_
/* Initialization Function List */
void initClocks();
void initPinIO();
void initUART();
void initADC10();
void initTimer();

#endif /* INIT_H_ */
