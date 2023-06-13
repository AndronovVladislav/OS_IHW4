#include "../Other/utils.h"

int servSock;
int clntAmount;

void ctrlCHandler(int nsig) {
    shm_unlink(QUEUE_REGION);
    exit(0);
}

void SIGIOHandler(int signalType) {    
    HandleUDPClient1(servSock, clntAmount);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage:  %s <Server Port> <Amount of Clients>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, ctrlCHandler);

    int clntSock;
    unsigned int childProcCount = 0;
    
    int echoServPort = atoi(argv[1]);
    clntAmount = atoi(argv[2]);
    servSock = CreateUDPServerSocket(echoServPort);

    struct sigaction handler;
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
    }
    
    shm_unlink(QUEUE_REGION);
    exit(0);
}