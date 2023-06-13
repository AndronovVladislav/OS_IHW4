#ifndef IHW3_UTILS_H
#define IHW3_UTILS_H

#define SELLERS_AMOUNT     3
#define QUEUE_REGION       "queue"

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
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct client {
    int currentSeller;
    int sellersToVisit[3];
} client;

void DieWithError(char *errorMessage);
void HandleUDPClient1(int clntSocket, int clientsAmount);
void HandleUDPClient2(int sock, int clientsAmount, struct sockaddr_in observerAddr);
int CreateUDPServerSocket(unsigned short port);
void PrintClients(int clientsAmount);
void CreateClients(int clientsAmount);
int UnservedClients(int clientsAmount);

#endif //IHW3_UTILS_H
