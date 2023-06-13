#include "../Other/utils.h"


void (*prevHandler)(int);
pid_t processID;                 /* Process ID from fork() */
int servSock;
int clntAmount;

void ctrlCHandler(int nsig) {
    shm_unlink(QUEUE_REGION);
    exit(0);
}

void SIGIOHandler(int signalType) {    
    // struct sockaddr_in echoClntAddr;  /* Address of datagram source */
    // unsigned int clntLen;             /* Address length */
    // int recvMsgSize;                  /* Size of datagram */
    // char echoBuffer[ECHOMAX];         /* Datagram buffer */

    // do  /* As long as there is input... */ {
    //     /* Set the size of the in-out parameter */
    //     clntLen = sizeof(echoClntAddr);

    //     if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
    //            (struct sockaddr *) &echoClntAddr, &clntLen)) < 0) {
    //         /* Only acceptable error: recvfrom() would have blocked */
    //         if (errno != EWOULDBLOCK)  
    //             DieWithError("recvfrom() failed");
    //     } else {
    //         printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    //         if (sendto(sock, echoBuffer, recvMsgSize, 0, (struct sockaddr *) 
    //               &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
    //             DieWithError("sendto() failed");
    //     }
    // } while (recvMsgSize >= 0);
    // /* Nothing left to receive */

    // if ((processID = fork()) < 0) {
    //     DieWithError("fork() failed");
    // } else if (processID == 0) {
    //     signal(SIGINT, prevHandler);

        // close(servSock);   /* Child closes parent socket */
        HandleTCPClient1(servSock, clntAmount);
        // exit(0);           /* Child process terminates */
    // }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage:  %s <Server Port> <Amount of Clients>\n", argv[0]);
        exit(1);
    }

    prevHandler = signal(SIGINT, ctrlCHandler);

    int clntSock;                    /* Socket descriptor for client */
    unsigned int childProcCount = 0; /* Number of child processes */ 
    
    int echoServPort = atoi(argv[1]);  /* First arg:  local port */
    clntAmount = atoi(argv[2]);
    servSock = CreateTCPServerSocket(echoServPort);

    struct sigaction handler;        /* Signal handling action definition */
    handler.sa_handler = SIGIOHandler;
    if (sigfillset(&handler.sa_mask) < 0) 
        DieWithError("sigfillset() failed");
    handler.sa_flags = 0;

    if (sigaction(SIGIO, &handler, 0) < 0)
        DieWithError("sigaction() failed for SIGIO");

    if (fcntl(servSock, F_SETOWN, getpid()) < 0)
        DieWithError("Unable to set process owner to us");

    if (fcntl(servSock, F_SETFL, O_NONBLOCK | FASYNC) < 0)
        DieWithError("Unable to put client sock into non-blocking/async mode");

    CreateClients(clntAmount);

    for (;;) {
        // if ((processID = fork()) < 0) {
        //     DieWithError("fork() failed");
        // } else if (processID == 0) {
        //     signal(SIGINT, prevHandler);

        //     // close(servSock);   /* Child closes parent socket */
        //     HandleTCPClient1(servSock, clntAmount);
        //     exit(0);           /* Child process terminates */
        // }
        // childProcCount++;      /* Increment number of outstanding child processes */
        // printf("with child process: %d\n", processID);
        // close(clntSock);       /* Parent closes child socket descriptor */

        // while (childProcCount) {
        //     processID = waitpid((pid_t) -1, NULL, WNOHANG);  /* Non-blocking wait */
        //     // printf("%d %d\n", childProcCount, processID);
        //     if (processID < 0)  /* waitpid() error? */ {
        //         DieWithError("waitpid() failed");
        //     } else if (processID == 0)  /* No zombie to wait on */ {
        //         break;
        //     } else {
        //         childProcCount--;  /* Cleaned up after a child */
        //     }
        // }
        // continue;
    }
    
    shm_unlink(QUEUE_REGION);
    exit(0);
}