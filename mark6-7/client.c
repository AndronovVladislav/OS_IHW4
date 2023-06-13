#include "../Other/utils.h"

int main(int argc, char *argv[]) {
    if ((argc < 4) || (argc > 6)) {
       fprintf(stderr, "Usage: %s <Server IP> <Multicast IP> <Number of Seller> [<Send Port>] [<Receive Port>]\n",
               argv[0]);
       exit(1);
    }

    // common part
    int bytesRcvd, totalBytesRcvd;                                  /* Bytes read in single recv() and total bytes read */
    int sellerNumber = atoi(argv[3]);
    client clientBuffer;                                            /* Buffer for echo string */

    // sender part
    int sendSock;                                                   /* Socket to send data */
    char *servIP = argv[1];
    unsigned short sendPort = argc >= 5 ? atoi(argv[4]) : 5000;     /* Port to send data */

    struct sockaddr_in sendAddr;                                    /* Echo server address */
    struct sockaddr_in fromAddr;                                    /* Source address of echo */
    unsigned int fromSize = sizeof(fromAddr);                       /* In-out of address size for recvfrom() */

    if ((sendSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    memset(&sendAddr, 0, sizeof(sendAddr));                         /* Zero out structure */
    sendAddr.sin_family         = AF_INET;                          /* Internet addr family */
    sendAddr.sin_addr.s_addr    = inet_addr(servIP);                /* Server IP address */
    sendAddr.sin_port           = htons(sendPort);                  /* Server port */
    // end of sender port

    // receiver part
    int receiveSock;                                                /* Socket to receive data*/
    char *receiveIP = argv[2];
    unsigned short receivePort = argc == 6 ? atoi(argv[5]) : 8000;  /* Port to send data */

    struct sockaddr_in multicastAddr;                               /* Multicast Address */
    struct ip_mreq multicastRequest;                                /* Multicast address join structure */

    if ((receiveSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        DieWithError("socket() failed");
    }

    memset(&multicastAddr, 0, sizeof(multicastAddr));               /* Zero out structure */
    multicastAddr.sin_family        = AF_INET;                      /* Internet address family */
    multicastAddr.sin_addr.s_addr   = htonl(INADDR_ANY);            /* Any incoming interface */
    multicastAddr.sin_port          = htons(receivePort);           /* Multicast port */

    if (bind(receiveSock, (struct sockaddr *) &multicastAddr, sizeof(multicastAddr)) < 0)
        DieWithError("bind() failed");

    multicastRequest.imr_multiaddr.s_addr = inet_addr(receiveIP);
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(receiveSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &multicastRequest,
          sizeof(multicastRequest)) < 0)
        DieWithError("setsockopt() failed");
    // end of receiver part

    totalBytesRcvd = 0;
    while (totalBytesRcvd < sizeof(sellerNumber)) {
        if (sendto(sendSock, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &sendAddr, sizeof(sendAddr)) != sizeof(sellerNumber)) {
            DieWithError("send() failed");
        }
        printf("I sent %d seller number\n", sellerNumber);

        if ((bytesRcvd = recvfrom(receiveSock, &sellerNumber, sizeof(sellerNumber), 0, NULL, 0)) <= 0) {
            DieWithError("recv() failed or connection closed prematurely");
        }
        printf("I received %d seller number\n", sellerNumber);

        totalBytesRcvd += bytesRcvd; /* Keep tally of total bytes */
    }

    // close(sock);
    exit(0);
}
