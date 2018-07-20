/*
 * helper.h
 *
 *  Created on: Jul 19, 2018
 *      Author: rscovill
 */

#ifndef PONG_HELPER_H_
#define PONG_HELPER_H_

#include <time.h>
#define STRING_MAX 1024
#define CAPE_PATH "/sys/devices/platform/bone_capemgr/slots"

// Read file into string
void Helper_readFromFile(char *fileName, char *buff);

// Write a string to file
void Helper_writeStringToFile(char *fileName, char *value);

// sleep for ms
void Helper_milliSleep(int ms);


#endif /* PONG_HELPER_H_ */
