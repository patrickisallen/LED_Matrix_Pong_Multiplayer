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
//#include "udpclient.h"
#include <math.h>
// #include "joystickControl.h"
// #include "accelController.h"

#define BUFSIZE 1024
#define SHORT_MSG_BUFFER 40
#define NUM_ARGS 2
#define BITS_FOR_ARR_TO_STR 15 //32-bit INT = 12 bits + 2 bits for ',' & ' ' + 1
#define NUM_DIGITS_MAX 3

_Bool keepRunFlag = true;
pthread_t t2;
int sockfd;
int portno;
socklen_t addr_size;
struct sockaddr_in serveraddr;
struct sockaddr_in clientaddr;
char *buf;
char *bufCpy;
char *bufArr[NUM_ARGS];
int reset;
int n;
int argStr2Int;
char *ptr;
int sizeNums = sizeof(int) * NUM_DIGITS_MAX;

int sendDatagram(char *inputStr) {
    int inputLen = strlen(inputStr) + 1; //+1 for additional null terminating character(s)
    int q = sendto(sockfd, inputStr, inputLen, 0,
                (struct sockaddr *)&clientaddr, addr_size);
    if (q < 0) { perror("ERROR in sendto"); }
    printf("Message sent!\n");
    return q;
}


void appToEnd(char *inputStr, char lineBreak) {
    int inputLen = strlen(inputStr);
    inputStr[inputLen] = lineBreak;
    inputStr[inputLen+1] = '\0';
}

void UDP_stop() {
    pthread_join(t2, NULL);
    keepRunFlag = false;
}

void UDP_client() {
    portno = 12345;

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


    //------------------------------------------------------
    buf = malloc((BUFSIZE+1) * sizeof(char)); // +1 for additioinal terminating characters
    bufCpy = malloc((BUFSIZE+1) * sizeof(char));

    	while (keepRunFlag) {
            memset(buf, '\0', BUFSIZE); //Clear buffer / fill it with null terminating characters
            n = recvfrom(sockfd, buf, BUFSIZE, 0,
                    (struct sockaddr *)&clientaddr, &addr_size);
            if (n < 0) { perror("ERROR in recvfrom"); }

            //Parse buffer
            strcpy(bufCpy, buf);
            char *p = strtok (bufCpy, " ");
            bufArr[0] = p;
            p = strtok (NULL, " ");
            bufArr[1] = p;

             if(bufArr[1] != NULL) {
                 argStr2Int = (int) strtol(bufArr[1], &ptr, 10);
            }


            if(strcmp(buf, "stop") == 0 || strcmp(buf,"stop\n") == 0) {
                UDP_stop();

            }
	    }
    free(bufCpy);
    free(buf);
}

void UDP_init() {
    int ret2 = pthread_create(&t2, NULL, (void *) &UDP_client, NULL);
    if(ret2) {
        fprintf(stderr, "Error - pthread_create() return code: %d\n", ret2);
    }
}


// int main() {
//   printf("\n\nTo connect to UDP server, open new terminal with the following command:\n");
//      printf("nc -u 192.168.7.2 12345\n\n");
//   UDP_init();
//   UDP_stop();
//
// }
