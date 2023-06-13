#ifndef IHW3_UTILS_H
#define IHW3_UTILS_H

#define SELLERS_AMOUNT 3
#define QUEUE_REGION "queue"

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PRINT_GREEN(X) printf("%s%d%s ", ANSI_COLOR_GREEN, X, ANSI_COLOR_RESET)

#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

typedef struct client {
    int currentSeller;
    int sellersToVisit[3];
} client;

#define MAXPENDING 5    /* Maximum outstanding connection requests */

void DieWithError(char *errorMessage);  /* Error handling function */
void HandleTCPClient1(int clntSocket, int clientsAmount);   /* TCP client handling function */
void HandleTCPClient2(int sock, int clientsAmount, struct sockaddr_in* multicastAddr);   /* TCP client handling function */
int CreateTCPServerSocket(unsigned short port); /* Create TCP server socket */
int AcceptTCPConnection(int servSock);  /* Accept TCP connection request */
void PrintClients(int clientsAmount);
void CreateClients(int clientsAmount);
int UnservedClients(int clientsAmount);
int HandleUDPObserver();
// void SIGIOHandler(int signalType);

#endif //IHW3_UTILS_H
