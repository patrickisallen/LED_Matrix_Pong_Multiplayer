/*
 * display.c
 *
 *  Created on: Jun 12, 2018
 *      Author: ryan
 *  i2c code taken from segDisplay.c on cmpt 433 website
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include "helper.h"
#include "gpio.h"
#include "main.h"

#define CAPE_ID "BB-I2C1"

#define GPIO_LEFT_DIGIT "61"
#define GPIO_RIGHT_DIGIT "44"
#define MAX_NUM 99
#define NUM_BASE 10
#define NUM_OF_DISPLAY_VALUES 10

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

typedef struct DisplayValue
{
	char digit;
	unsigned int registerAValue;
	unsigned int registerBValue;
} DisplayValue;


static _Bool running = false;
static pthread_t pthreadDisplay;
static int i2cFileDesc = 0;

static char charA = '0'; // left digit
static char charB = '0'; // right digit

const static DisplayValue displayValues[NUM_OF_DISPLAY_VALUES] = {
	{'0', 0xA1, 0x86},
	{'1', 0x80, 0x12},
	{'2', 0x31, 0x0E},
	{'3', 0xB0, 0x06},
	{'4', 0x90, 0x8A},
	{'5', 0xB0, 0x8C},
	{'6', 0xB1, 0x8C},
	{'7', 0x04, 0x14},
	{'8', 0xB1, 0x8E},
	{'9', 0xB0, 0x8E},
};

static int initI2cBus(char *bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
static void *runDisplay();

static void initGPIO(void)
{
	GPIO_export(GPIO_LEFT_DIGIT);
	GPIO_export(GPIO_RIGHT_DIGIT);
	GPIO_setDirection(GPIO_LEFT_DIGIT, "out");
	GPIO_setDirection(GPIO_RIGHT_DIGIT, "out");
	GPIO_setValue(GPIO_LEFT_DIGIT, "1");
	GPIO_setValue(GPIO_RIGHT_DIGIT, "1");
}

static void shutdownGPIO(void)
{
	GPIO_setValue(GPIO_LEFT_DIGIT, "0");
	GPIO_setValue(GPIO_RIGHT_DIGIT, "0");
}

void Display_init(void)
{

	initGPIO();

	Helper_writeStringToFile(CAPE_PATH, CAPE_ID);
	i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

	writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
	writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);

	running = true;
	if(pthread_create(&pthreadDisplay, NULL, &runDisplay, NULL)) {
		printf("Error creating mutex in display.c! Error %s\n", strerror(errno));
		exit(1);
	}
}

void Display_shutdown(void)
{
	running = false;
	shutdownGPIO();
	pthread_join(pthreadDisplay, 0);
	// Cleanup I2C access;
	close(i2cFileDesc);
}

static int initI2cBus(char *bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0)
	{
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		Main_triggerShutdown();
		exit(1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0)
	{
		perror("Unable to set I2C device to slave address.");
		Main_triggerShutdown();
		exit(1);
	}
	return i2cFileDesc;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2)
	{
		perror("Unable to write i2c register");
		Main_triggerShutdown();
		exit(1);
	}
}

static void charToRegs(char c, unsigned int *regA, unsigned int *regB)
{
	for(int i = 0; i < NUM_OF_DISPLAY_VALUES; i++) {
		DisplayValue value = displayValues[i];
		if(value.digit == c) {
			*regA = value.registerAValue;
			*regB = value.registerBValue;
			return;
		}
	}


	//if we didn't find the character in the array use default values
	*regA = 0x00;
	*regB = 0x00;
}

static void displayChar(char c)
{
	unsigned int regA = 0x00;
	unsigned int regB = 0x00;
	charToRegs(c, &regA, &regB);
	writeI2cReg(i2cFileDesc, REG_OUTA, regA);
	writeI2cReg(i2cFileDesc, REG_OUTB, regB);
}

void Display_num(long long num)
{
	if (num > MAX_NUM)
	{
		num = MAX_NUM;
	}
	if (num < NUM_BASE)
	{
		charA = '0';
		charB = (int)num + '0';
	}
	else
	{
		charA = (int)(num / NUM_BASE) + '0';
		charB = (int)(num % NUM_BASE) + '0';
	}
}

// thread for display
static void *runDisplay(void)
{
	while (running)
	{
		// alternate displaying characters on left and right digits
		displayChar(charA);
		GPIO_setValue(GPIO_LEFT_DIGIT, "1");
		Helper_milliSleep(5);
		GPIO_setValue(GPIO_LEFT_DIGIT, "0");
		displayChar(charB);
		GPIO_setValue(GPIO_RIGHT_DIGIT, "1");
		Helper_milliSleep(5);
		GPIO_setValue(GPIO_RIGHT_DIGIT, "0");
	}
	return 0;
}
