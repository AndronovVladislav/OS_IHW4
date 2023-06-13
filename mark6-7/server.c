#include "../Other/utils.h"

int clntAmount;

int sock;
struct sockaddr_in observerAddr;

void ctrlCHandler(int nsig) {
    shm_unlink(QUEUE_REGION);
    exit(0);
}

void SIGIOHandler(int signalType) {
    HandleUDPClient2(sock, clntAmount, observerAddr);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage:  %s <Server Port> <Amount of Clients>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, ctrlCHandler);

    clntAmount = atoi(argv[2]);
    unsigned short port = atoi(argv[1]);

    char *servIP;
    struct sockaddr_in servAddr;
    unsigned int fromSize = sizeof(observerAddr);

    sock = CreateUDPServerSocket(port);
    CreateClients(clntAmount);

    int to_receive_from_observer;
    if (recvfrom(sock, &to_receive_from_observer, sizeof(to_receive_from_observer), 0, (struct sockaddr *)
        &observerAddr, &fromSize) != sizeof(to_receive_from_observer))
        DieWithError("Can't obtain observer address");
    printf("Connection with the observer is established\n");


    struct sigaction handler;
    handler.sa_handler = SIGIOHandler;
    if (sigfillset(&handler.sa_mask) < 0) 
        DieWithError("sigfillset() failed");
    handler.sa_flags = 0;

    if (sigaction(SIGIO, &handler, 0) < 0)
        DieWithError("sigaction() failed for SIGIO");

    if (fcntl(sock, F_SETOWN, getpid()) < 0)
        DieWithError("Unable to set process owner to us");

    if (fcntl(sock, F_SETFL, O_NONBLOCK | FASYNC) < 0)
        DieWithError("Unable to put client sock into non-blocking/async mode");

    for (;;) {
    }

    shm_unlink(QUEUE_REGION);
    exit(0);
}