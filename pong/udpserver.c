#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>

#include "udpserver.h"
#include "pong.h"

#define BUFSIZE 1024
#define SHORT_MSG_BUFFER 40
#define NUM_ARGS 2
#define BITS_FOR_ARR_TO_STR 15 //32-bit INT = 12 bits + 2 bits for ',' & ' ' + 1 

_Bool keepRunningFlag = true;
pthread_t t2;
static int sockfd;
static int portno;
socklen_t addr_size;
struct sockaddr_in serveraddr;
struct sockaddr_in clientaddr;
static char *buf;
static char *bufCpy;
static int reset;
static int playerID;
pthread_mutex_t copyLock;

int sendDatagram(char *inputStr) {
    int inputLen = strlen(inputStr) + 1; //+1 for additional null terminating character(s)
    int characters_sent = sendto(sockfd, inputStr, inputLen, 0,
                (struct sockaddr *)&clientaddr, addr_size);
    if (characters_sent < 0) { 
        perror("ERROR in sendto"); 
    }
    return characters_sent;
}

void UDP_stop() {
    pthread_join(t2, NULL);
    keepRunningFlag = false;
}

void UDP_server() {
	if (playerID == 1) {
		portno = 12345;
	} else {
		portno = 12346;
	}

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("ERROR OPENING SOCKET!\n");
        exit(1);
    }

    //Reset port & server when killing program
    reset = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &reset, sizeof(int));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);
    addr_size = sizeof(clientaddr);

    if(bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("ERROR BINDING PORT\n");
        exit(1);
    }

    char *help = "I am a help message";
    char *invalidMsg = "I am an invalid message";


    //------------------------------------------------------
    buf = malloc((BUFSIZE+1) * sizeof(char)); // +1 for additioinal terminating characters 
    bufCpy = malloc((BUFSIZE+1) * sizeof(char));
    int bytes_received = 0;
    while (keepRunningFlag) {        
		bytes_received = recvfrom(sockfd, buf, BUFSIZE, 0,
			     (struct sockaddr *)&clientaddr, &addr_size);
		if (bytes_received < 0) {
             perror("ERROR in recvfrom"); 
        }
		printf("server received %d bytes\n", bytes_received);

        buf[bytes_received + 1] = '\0';
        //Parse buffer
        if(pthread_mutex_lock(&copyLock)) {
            printf("Error locking mutex in udpserver.c! Error %s\n", strerror(errno));
		    exit(1);
        }

        strcpy(bufCpy, buf);
        if(pthread_mutex_unlock(&copyLock)) {
            printf("Error unlocking mutex in udpserver.c! Error %s\n", strerror(errno));
		    exit(1);
        }

        if(strcmp(buf, "0") == 0) {
        	if (playerID == 1) {
        		Pong_movePaddle(2, 0);
        	}else {
        		Pong_movePaddle(1, 0);
        	}
        }
        if(strcmp(buf, "1") == 0) {
			if (playerID == 1) {
				Pong_movePaddle(2, 1);
			}else {
				Pong_movePaddle(1, 1);
			}
		}

        if(strcmp(buf, "r") == 0) {
            Pong_increaseReadyCount();
        }

        if(strcmp(buf, "reset\n") == 0) {
            Pong_resetGame();
        }

        if(strcmp(buf, "help\n") == 0) {
            sendDatagram(help);
        } else {
            sendDatagram(invalidMsg);
        }
	}
    free(bufCpy);
    free(buf);
}

int UDP_get_buff() {
	int joyVal;
	if (bufCpy != NULL) {
		joyVal = atoi(bufCpy);
	} else {
		joyVal = -1;
	}
    bufCpy = NULL;
    return joyVal;
}


void UDP_init(int player) {
	playerID = player;
    if (pthread_mutex_init(&copyLock, NULL) != 0) {
        printf("Init lock has failed\n");
        exit(1);
    }
    int ret2 = pthread_create(&t2, NULL, (void *) &UDP_server, NULL);
    if(ret2) {
        fprintf(stderr, "Error - pthread_create() return code: %d\n", ret2);
        exit(1);
    }
}
