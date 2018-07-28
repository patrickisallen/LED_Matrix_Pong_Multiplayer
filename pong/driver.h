/*
 * driver.h
 *
 *  Created on: July 28, 2018
 *      Author: Scott
 *
 * Module to interact with the GPIO pins on the beaglebone.
 */

#ifndef DRIVER_H_
#define DRIVER_H_

#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 16

#define COLOUR_RED 1
#define COLOUR_GREEN 2
#define COLOUR_YELLOW 3
#define COLOUR_BLUE 4
#define COLOUR_MAGENTA 5
#define COLOUR_CYAN 6
#define COLOUR_WHITE 7

#define DRIVER_FILE "/dev/matrix"

#include <stdbool.h>


bool Driver_init();
void Driver_writeData(int data[SCREEN_WIDTH][SCREEN_HEIGHT]);
void Driver_clear();
void Driver_deinit();

#endif /* DRIVER_H_ */
