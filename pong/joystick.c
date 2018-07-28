/*
 * joystick.c
 *
 *  Created on: Jul 3, 2018
 *      Author: ryan
 */

#include "joystick.h"
#include "helper.h"
#include "gpio.h"
#include "pthread.h"

#define GPIO_UP "26"
#define GPIO_DOWN "46"
#define GPIO_LEFT "65"
#define GPIO_RIGHT "47"
#define GPIO_CENTER "27"

static int readStick(int pos, char *gpio, char *buff)
{
	GPIO_getValue(gpio, buff);
	return 0;
}

int Joystick_init()
{

	GPIO_export(GPIO_UP);
	GPIO_export(GPIO_DOWN);
	GPIO_export(GPIO_LEFT);
	GPIO_export(GPIO_RIGHT);
	GPIO_export(GPIO_CENTER);
	return 0;
}

int Joystick_getDirection()
{
	char buff[STRING_MAX];
	readStick(UP, GPIO_UP, buff);
	if (*buff == '0'){
		return UP;
	}
	readStick(DOWN, GPIO_DOWN, buff);
	if (*buff == '0'){
		return DOWN;
	}
	readStick(LEFT, GPIO_LEFT, buff);
	if (*buff == '0'){
		return LEFT;
	}
	readStick(RIGHT, GPIO_RIGHT, buff);
	if (*buff == '0'){
		return RIGHT;
	}
	readStick(CENTER, GPIO_CENTER, buff);
	if (*buff == '0'){
		return CENTER;
	}
	return NONE;
}
