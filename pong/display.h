/*
 * display.h
 *
 *  Created on: Jun 12, 2018
 *      Author: ryan
 *
 * Module that interacts with the 14 character display on the Zencape.
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

// Initialize the display
void Display_init(void);

// Clean up the display and turn off
void Display_shutdown(void);

// Display a number
void Display_num(long long num);

#endif /* DISPLAY_H_ */
