/*
 * gpio.h
 *
 *  Created on: Jun 12, 2018
 *      Author: ryan
 *
 * Module to interact with the GPIO pins on the beaglebone.
 */

#ifndef GPIO_H_
#define GPIO_H_

// exports specified GPIO Pin
void GPIO_export(char* pin);

// sets the value of GPIO Pin
void GPIO_setValue(char* pin, char* value);

// gets the value of GPIO file
void GPIO_getValue(char* pin, char* value);

// sets the direction of GPIO Pin
void GPIO_setDirection(char* pin, char* direction);

#endif /* GPIO_H_ */
