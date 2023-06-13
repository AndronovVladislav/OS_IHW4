#include "../Other/utils.h"

int main(int argc, char *argv[]) {
    if ((argc < 3) || (argc > 4)) {
       fprintf(stderr, "Usage: %s <Server IP> <Number of Seller> [<Send Port>]\n",
               argv[0]);
       exit(1);
    }

    int bytesRcvd, totalBytesRcvd;
    int sellerNumber = atoi(argv[2]);
    client clientBuffer;

    int sock;
    char *servIP = argv[1];
    unsigned short sendPort = argc == 4 ? atoi(argv[3]) : 5000;

    struct sockaddr_in sendAddr;
    struct sockaddr_in fromAddr;
    unsigned int fromSize = sizeof(fromAddr);

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    memset(&sendAddr, 0, sizeof(sendAddr));
    sendAddr.sin_family         = AF_INET;
    sendAddr.sin_addr.s_addr    = inet_addr(servIP);
    sendAddr.sin_port           = htons(sendPort);

    totalBytesRcvd = 0;
    while (totalBytesRcvd < sizeof(sellerNumber)) {
        if (sendto(sock, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &sendAddr, sizeof(sendAddr)) != sizeof(sellerNumber)) {
            DieWithError("send() failed");
        }
        printf("I sent %d seller number\n", sellerNumber);

        if ((bytesRcvd = recvfrom(sock, &sellerNumber, sizeof(sellerNumber), 0, NULL, 0)) <= 0) {
            DieWithError("recv() failed or connection closed prematurely");
        }
        printf("I received %d seller number\n", sellerNumber);

        totalBytesRcvd += bytesRcvd;
    }

    close(sock);
    exit(0);
}
