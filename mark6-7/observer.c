#include "../Other/utils.h"

int main(int argc, char *argv[]) {
    if ((argc < 3) || (argc > 4)) {
       fprintf(stderr, "Usage: %s <Multicast IP> <Amount of Clients> [<Echo Port>]\n",
               argv[0]);
       exit(1);
    }

    // receiver part
    int receiveSock;                                                /* Socket to receive data*/
    char *receiveIP = argv[1];
    unsigned short receivePort = argc == 4 ? atoi(argv[3]) : 8000;  /* Port to send data */

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

    // int sock;                        /* Socket descriptor */
    // struct sockaddr_in echoServAddr; /* Echo server address */
    // client clientBuffer;             /* Buffer for echo string */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv()
    //                                     and total bytes read */

    // char *servIP = argv[1];
    int sellerNumber;
    // int echoServPort = argc == 4 ? atoi(argv[3]) : 7;

    struct sockaddr_in fromAddr;                        /* Source address of echo */
    unsigned int fromSize = sizeof(fromAddr);           /* In-out of address size for recvfrom() */

    // /* Create a reliable, stream socket using TCP */
    // if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    //     DieWithError("socket() failed");
    // }

    // /* Construct the server address structure */
    // memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    // echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    // echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    // echoServAddr.sin_port        = htons(echoServPort); /* Server port */

    totalBytesRcvd = 0;
    // printf("%sStart%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
    // PrintClients(atoi(argv[2]));
    for (;;) {
        // int stop = !UnservedClients(atoi(argv[2])) ? 1 : 0;
        // if (stop) {
        //     if (sendto(sock, &stopSignal, sizeof(stopSignal), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(stop)) {
        //         DieWithError("send() failed");
        //     }
        //     break;
        // } else {
        //     if (sendto(sock, &usualSignal, sizeof(usualSignal), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(stop)) {
        //         DieWithError("send() failed");
        //     }
        // }

        if (recvfrom(receiveSock, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &fromAddr, &fromSize) <= 0) {
            DieWithError("recv() failed or connection closed prematurely");
        }

        if (sellerNumber < 255) {
            printf("%sAfter %d seller's work%s\n", ANSI_COLOR_CYAN, sellerNumber, ANSI_COLOR_RESET);
            PrintClients(atoi(argv[2]));
        }

        sleep(3);
        // int stop = !UnservedClients(atoi(argv[2])) ? 1 : 0;
        // if (sendto(sock, &stop, sizeof(stop), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(stop)) {
        //     DieWithError("send() failed");
        // }

        // if (stop) {
        //     printf("%sFinish%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
        //     // break;
        // }
    }

    // close(sock);
    exit(0);
}