/*
 * joystick.h
 *
 *  Created on: Jul 3, 2018
 *      Author: ryan
 *
 *  Module to interact with the joystick
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

enum StickPos {
	NONE, UP, DOWN, LEFT, RIGHT, CENTER
};

// initialize the joystick
int Joystick_init();

// get the direction of the joy stick position
int Joystick_getDirection();



#endif /* JOYSTICK_H_ */
