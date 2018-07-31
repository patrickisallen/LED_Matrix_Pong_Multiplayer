#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

static int clientSocket;
struct sockaddr_in serverAddr;
socklen_t addr_size;

void UDP_send_message(char *buf)
{
    int bufSendSize = strlen(buf) + 1;
    int characters_sent = sendto(clientSocket, buf, bufSendSize, 0, (struct sockaddr *)&serverAddr, addr_size);
    if (characters_sent < 0) { 
        perror("ERROR in sendto"); 
    }
}

void UDP_client_init(int player)
{
    printf("Initializing UDP client\n");
    /*Create UDP socket*/
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

    char *server_name;
    int portNum = 0;

    if (player == 1)
    {
        server_name = "169.254.0.1";
        portNum = 12346;
    }
    else
    {
        server_name = "169.254.0.2";
        portNum = 12345;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNum);
    inet_pton(AF_INET, server_name, &serverAddr.sin_addr);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*Initialize size variable to be used later on*/
    addr_size = sizeof serverAddr;
}
