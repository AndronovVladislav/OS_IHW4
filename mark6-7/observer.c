#include "../Other/utils.h"

int main(int argc, char *argv[]) {
    if ((argc < 3) || (argc > 4)) {
       fprintf(stderr, "Usage: %s <Server IP> <Amount of Clients> [<Send Port>]\n",
               argv[0]);
       exit(1);
    }

    int sock;
    struct sockaddr_in echoServAddr;
    client clientBuffer;
    int bytesRcvd, totalBytesRcvd;

    char *servIP = argv[1];
    int sellerNumber;
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

    int to_send_observer_address;
    if (sendto(sock, &to_send_observer_address, sizeof(to_send_observer_address), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != sizeof(to_send_observer_address)) {
        DieWithError("send() failed");
    }

    sleep(atoi(argv[2]));

    totalBytesRcvd = 0;
    printf("%sStart%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
    PrintClients(atoi(argv[2]));
    for (;;) {
        if (recvfrom(sock, &sellerNumber, sizeof(sellerNumber), 0, NULL, NULL) <= 0) {
            DieWithError("recv() failed or connection closed prematurely");
        }

        if (sellerNumber < 255) {
            printf("%sAfter %d seller's work%s\n", ANSI_COLOR_CYAN, sellerNumber, ANSI_COLOR_RESET);
            PrintClients(atoi(argv[2]));
        }

        int stop = !UnservedClients(atoi(argv[2])) ? 1 : 0;
        if (stop) {
            printf("%sFinish%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
            break;
        }
    }

    exit(0);
}