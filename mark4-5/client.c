#include "../Other/utils.h"

int main(int argc, char *argv[]) {
    if ((argc < 3) || (argc > 4)) {
       fprintf(stderr, "Usage: %s <Server IP> <Number of Seller> [<Echo Port>]\n",
               argv[0]);
       exit(1);
    }

    int sock;
    struct sockaddr_in echoServAddr;
    client clientBuffer;
    int bytesRcvd, totalBytesRcvd;

    char *servIP = argv[1];
    int sellerNumber = atoi(argv[2]);
    int echoServPort = argc == 4 ? atoi(argv[3]) : 7;

    struct sockaddr_in fromAddr;
    unsigned int fromSize = sizeof(fromAddr);

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        DieWithError("socket() failed");
    }

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family      = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port        = htons(echoServPort);

    totalBytesRcvd = 0;
    while (totalBytesRcvd < sizeof(sellerNumber)) {
        if (sendto(sock, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(sellerNumber)) {
            DieWithError("send() failed");
        }
        printf("I sent %d seller number\n", sellerNumber);
        

        if ((bytesRcvd = recvfrom(sock, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &fromAddr, &fromSize)) <= 0) {
            DieWithError("recv() failed or connection closed prematurely");
        }
        printf("I received %d seller number\n", sellerNumber);

        totalBytesRcvd += bytesRcvd;
    }

    close(sock);
    exit(0);
}
