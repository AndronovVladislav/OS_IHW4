#include "../Other/utils.h"

int sock;                         /* Socket */
char *servIP;                     /* Server IP address */

// common part
void (*prevHandler)(int);
int clntAmount;


// sender part
int sendSock;                                                   /* Socket to send data */
struct sockaddr_in multicastAddr;                               /* Multicast Address */


void ctrlCHandler(int nsig) {
    shm_unlink(QUEUE_REGION);
    exit(0);
}

void SIGIOHandler(int signalType) {
    HandleTCPClient2(sock, clntAmount, &multicastAddr);
}

int main(int argc, char *argv[]) {
    if ((argc < 4) || (argc > 5)) {
        fprintf(stderr, "Usage:  %s <Server IP> <Multicast IP> [<Send Port>] [<Receive Port>] [<TTL>]\n",
                 argv[0]);
        exit(1);
    }

    // sender part
    int sendSock;                                                   /* Socket to receive data*/
    unsigned short sendPort = argc >= 4 ? atoi(argv[3]) : 8000;     /* Port to send data */

    struct ip_mreq multicastRequest;                                /* Multicast address join structure */
    unsigned char multicastTTL = argc == 6 ? atoi(argv[5]) : 5;     /* TTL of multicast packets */

    if ((sendSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        DieWithError("socket() failed");
    }

    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &multicastTTL,
          sizeof(multicastTTL)) < 0)
        DieWithError("setsockopt() failed");

    memset(&multicastAddr, 0, sizeof(multicastAddr));               /* Zero out structure */
    multicastAddr.sin_family        = AF_INET;                      /* Internet address family */
    multicastAddr.sin_addr.s_addr   = htonl(INADDR_ANY);            /* Any incoming interface */
    multicastAddr.sin_port          = htons(sendPort);              /* Multicast port */

    if (bind(sendSock, (struct sockaddr *) &multicastAddr, sizeof(multicastAddr)) < 0)
        DieWithError("bind() failed");

    multicastRequest.imr_multiaddr.s_addr = inet_addr(servIP);
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sendSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &multicastRequest,
          sizeof(multicastRequest)) < 0)
        DieWithError("setsockopt() failed");
    // end of sender port

    struct sigaction handler;        /* Signal handling action definition */
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

    CreateClients(clntAmount);
    sendStringLen = strlen(sendString);
    for (;;) {
        // if (sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
        //       &multicastAddr, sizeof(multicastAddr)) != sendStringLen)
        //     DieWithError("sendto() sent a different number of bytes than expected");
        // sleep(3);
    }
    
    shm_unlink(QUEUE_REGION);
    exit(0);
}