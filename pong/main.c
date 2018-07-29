/*
 * main.c
 *
 *  Created on: Jul 24, 2018
 *      Author: rscovill
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stddef.h>

#include "helper.h"
#include "pong.h"
#include "ledMatrix.h"
#include "joystick.h"
#include "display.h"
#include "udpserver.h"
#include "udpclient.h"

static pthread_mutex_t mainMutex = PTHREAD_MUTEX_INITIALIZER;

// initialize the module threads
static void init(int player)
{
	Pong_init(player);
	LEDMatrix_init();
	Joystick_init();
	Display_init();
	UDP_init(player);
	UDP_client_init(player);
}

static void terminate()
{

}

// Main thread
int main(int argc, char* args[])
{
	if(argc != 2) {
		printf("Error: player not selected! \
				Usage: ./pong playerNum to play.\n");
		return 1;
	}

	int player;
	sscanf (args[1],"%d",&player);

	if(player > 2 || player < 1) {
		printf("Error: Player must be either 1 or 2!\n");
		return -1;
	}

	// random seed
	srand(time(NULL));
	init(player);

	pthread_mutex_init(&mainMutex, NULL);
	// call the mutex lock twice so that the main thread waits until the lock is free
	pthread_mutex_lock(&mainMutex);
	pthread_mutex_lock(&mainMutex);
	terminate();
    return 0;
}

// Called by other threads to unlock the main thread
void Main_triggerShutdown()
{
	pthread_mutex_unlock(&mainMutex);
	pthread_mutex_unlock(&mainMutex);
}
