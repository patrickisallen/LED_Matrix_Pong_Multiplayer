/*
 * gpio.c
 *
 *  Created on: Jun 12, 2018
 *      Author: ryan
 */

#include "gpio.h"
#include "helper.h"
#include <string.h>

#define GPIO_PATH "/sys/class/gpio/gpio"
#define GPIO_EXPORT_PATH "/sys/class/gpio/export"

static void GPIO_setProp(char* pin, char* prop, char* value);
static void GPIO_getProp(char* pin, char* prop, char* value);

void GPIO_export(char* pin)
{
	Helper_writeStringToFile(GPIO_EXPORT_PATH, pin);
}

void GPIO_getValue(char* pin, char* value)
{
	GPIO_getProp(pin, "/value", value);
}

void GPIO_setValue(char* pin, char* value)
{
	GPIO_setProp(pin, "/value", value);
}

void GPIO_setDirection(char* pin, char* direction)
{
	GPIO_setProp(pin, "/direction", direction);
}

static void GPIO_setProp(char* pin, char* prop, char* value)
{
	char path[STRING_MAX] = GPIO_PATH;
	strcat(path, pin);
	strcat(path, prop);
	Helper_writeStringToFile(path, value);

}

static void GPIO_getProp(char* pin, char* prop, char* value)
{
	char path[STRING_MAX] = GPIO_PATH;
	strcat(path, pin);
	strcat(path, prop);
	Helper_readFromFile(path, value);
}
