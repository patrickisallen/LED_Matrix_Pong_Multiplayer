#include "driver.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

static FILE *pfile;

bool Driver_init()
{
    pfile = fopen(DRIVER_FILE, "w");
    if (pfile == NULL)
    {
        printf("ERROR: Unable to open file (%s) with error %s.\n", DRIVER_FILE, strerror(errno));
        return false;
    }

    return true;
}

void Driver_writeData(int data[SCREEN_WIDTH][SCREEN_HEIGHT])
{

    assert(pfile != NULL);
    int dataSize = SCREEN_WIDTH * SCREEN_HEIGHT;
    clock_t start = clock(), diff;
    size_t bytesWritten = fwrite(data, sizeof(int), dataSize, pfile);
    diff = clock() - start;
    if (bytesWritten != dataSize)
    {
        printf("ERROR: Unable to write dataSize %d to driver! Only %d bytes were written with error %s.\n",
               dataSize, bytesWritten, strerror(errno));
        exit(1);
    }

    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Time taken to write to driver %d seconds %d milliseconds\n", msec / 1000, msec % 1000);
}

void Driver_deinit()
{
    fclose(pfile);
}