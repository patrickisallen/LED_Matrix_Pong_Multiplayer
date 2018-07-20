#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * helper.c
 *
 *  Created on: Jul 19, 2018
 *      Author: rscovill
 */

// adapted from GPIO Guide notes
void Helper_readFromFile(char *fileName, char *buff)
{
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		printf("ERROR: Unable to open file (%s) for read\n", fileName);
		exit(-1);
	}
	// Read string (line)
	fgets(buff, STRING_MAX, file);
	// Close
	fclose(file);
}

// adapted from GPIO Guide notes
void Helper_writeStringToFile(char *fileName, char *value)
{
	FILE *pfile = fopen(fileName, "w");
	if (pfile == NULL) {
		printf("ERROR: Unable to open file (%s).\n", fileName);
		exit(1);
	}
	// Write to data to the file using fprintf():
	fprintf(pfile, "%s", value);
	// Close the file using fclose():
	fclose(pfile);
}

/* sleep for specified micro seconds */
void Helper_milliSleep(int ms)
{
	struct timespec tm;
	tm.tv_sec = (ms / 1000);
	tm.tv_nsec = (ms % 1000) * 1000000L;
	nanosleep(&tm, NULL);
}




