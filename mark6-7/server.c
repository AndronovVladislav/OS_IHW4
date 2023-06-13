#include "../Other/utils.h"

// common part
void (*prevHandler)(int);
int lastSellerNumber = -1;
int clntAmount;

// sender part
int sendSock;                                                   /* Socket to send data */
struct sockaddr_in multicastAddr;                               /* Multicast Address */

// receiver part
int receiveSock;                                                /* Socket to send data */
char *servIP;                     /* Server IP address */

void ctrlCHandler(int nsig) {
    shm_unlink(QUEUE_REGION);
    exit(0);
}

void SIGIOHandler(int signalType) {
        int sellerNumber;
    int shmid;
    struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen = sizeof(echoClntAddr);

    if ((shmid = shm_open(QUEUE_REGION, O_RDWR, S_IRWXU)) == -1) {
        perror("shm_open");
        exit(-1);
    }

    client* clients = mmap(0, sizeof(client) * clntAmount, PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);

    if (recvfrom(receiveSock, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen) < 0) {
        DieWithError("recvfrom() failed");
    }

    printf("I received %d\n", sellerNumber);

    if (sellerNumber == 5) {
        exit(0);
    }

    if (sellerNumber != 4) {
        printf("I received %d seller number\n", sellerNumber);

        for (int i = 0; i < clntAmount; ++i) {
            if (clients[i].currentSeller <= 2 && clients[i].sellersToVisit[clients[i].currentSeller] == sellerNumber) {
                sleep(rand() % 3 + 1);
                while (clients[i].currentSeller <= 2 && clients[i].sellersToVisit[++clients[i].currentSeller] == 0);
            }
        }
    }

    lastSellerNumber = sellerNumber;
    if (sendto(sendSock, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &multicastAddr, sizeof(multicastAddr)) != sizeof(sellerNumber)) {
        DieWithError("send() failed");
    }
    printf("I sent %d seller number to several hosts\n", sellerNumber);

    if (!UnservedClients(clntAmount)) {
        exit(0);
    }

    // HandleTCPClient2(receiveSock, clntAmount, sendSock, multicastAddr);
}

int main(int argc, char *argv[]) {
    if ((argc < 3) || (argc > 7)) {
        fprintf(stderr, "Usage:  %s <Server IP> <Multicast IP> <Number of Clients> [<Send Port>] [<Receive Port>] [<TTL>]\n",
                 argv[0]);
        exit(1);
    }

    clntAmount = atoi(argv[3]);

    // sender part
    unsigned short sendPort = argc >= 5 ? atoi(argv[4]) : 8000;     /* Port to send data */

    struct ip_mreq multicastRequest;                                /* Multicast address join structure */
    unsigned char multicastTTL = argc == 7 ? atoi(argv[6]) : 5;     /* TTL of multicast packets */

    if ((sendSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        DieWithError("socket() failed");
    }

    if (setsockopt(sendSock, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &multicastTTL,
          sizeof(multicastTTL)) < 0)
        DieWithError("setsockopt() failed");

    memset(&multicastAddr, 0, sizeof(multicastAddr));                   /* Zero out structure */
    multicastAddr.sin_family            = AF_INET;                      /* Internet address family */
    multicastAddr.sin_addr.s_addr       = inet_addr(argv[2]);           /* Multicast IP address */
    multicastAddr.sin_port              = htons(sendPort);              /* Multicast port */
    // end of sender port

    // receiver part
    unsigned short receivePort = argc >= 6 ? atoi(argv[5]) : 5000;     /* Port to send data */

    struct sockaddr_in receiveAddr;                                    /* Echo server address */
    struct sockaddr_in fromAddr;                                    /* Source address of echo */
    unsigned int fromSize = sizeof(fromAddr);                       /* In-out of address size for recvfrom() */

    if ((receiveSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    memset(&receiveAddr, 0, sizeof(receiveAddr));                         /* Zero out structure */
    receiveAddr.sin_family         = AF_INET;                          /* Internet addr family */
    receiveAddr.sin_addr.s_addr    = htonl(INADDR_ANY);                /* Server IP address */
    receiveAddr.sin_port           = htons(receivePort);                  /* Server port */

    if (bind(receiveSock, (struct sockaddr *) &receiveAddr, sizeof(receiveAddr)) < 0)
        DieWithError("bind() failed");
    // end of receiver port

    struct sigaction handler;        /* Signal handling action definition */
    handler.sa_handler = SIGIOHandler;
    if (sigfillset(&handler.sa_mask) < 0) 
        DieWithError("sigfillset() failed");
    handler.sa_flags = 0;

    if (sigaction(SIGIO, &handler, 0) < 0)
        DieWithError("sigaction() failed for SIGIO");

    if (fcntl(receiveSock, F_SETOWN, getpid()) < 0)
        DieWithError("Unable to set process owner to us");

    if (fcntl(receiveSock, F_SETFL, O_NONBLOCK | FASYNC) < 0)
        DieWithError("Unable to put client sock into non-blocking/async mode");

    CreateClients(clntAmount);
    for (;;) {
        if (sendto(sendSock, &lastSellerNumber, 1, 0, (struct sockaddr *)
              &multicastAddr, sizeof(multicastAddr)) != 1)
            DieWithError("sendto() sent a different number of bytes than expected");
        sleep(3);
    }
    
    shm_unlink(QUEUE_REGION);
    exit(0);
}