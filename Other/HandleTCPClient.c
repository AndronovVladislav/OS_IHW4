#include "utils.h"

void HandleTCPClient1(int clntSocket, int clientsAmount) {
    int sellerNumber;
    int shmid;
    struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen = sizeof(echoClntAddr);

    if ((shmid = shm_open(QUEUE_REGION, O_RDWR, S_IRWXU)) == -1) {
        perror("shm_open");
        exit(-1);
    }

    client* clients = mmap(0, sizeof(client) * clientsAmount, PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);

    if (recvfrom(clntSocket, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen) < 0) {
        DieWithError("recvfrom() failed");
    }
    printf("I received %d seller number\n", sellerNumber);

    for (int i = 0; i < clientsAmount; ++i) {
        if (clients[i].currentSeller <= 2 && clients[i].sellersToVisit[clients[i].currentSeller] == sellerNumber) {
            // sleep(rand() % 3 + 1);
            while (clients[i].currentSeller <= 2 && clients[i].sellersToVisit[++clients[i].currentSeller] == 0);
        }
    }

    // PrintClients(clientsAmount);

    if (sendto(clntSocket, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != sizeof(sellerNumber)) {
        DieWithError("send() failed");
    }
    printf("I sent %d seller number\n", sellerNumber);

    // close(clntSocket);
}

void HandleTCPClient2(int sock, int clientsAmount, struct sockaddr_in* multicastAddr) {
    int sellerNumber;
    int shmid;
    struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen = sizeof(echoClntAddr);

    if ((shmid = shm_open(QUEUE_REGION, O_RDWR, S_IRWXU)) == -1) {
        perror("shm_open");
        exit(-1);
    }

    client* clients = mmap(0, sizeof(client) * clientsAmount, PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);

    if (recvfrom(sock, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen) < 0) {
        DieWithError("recvfrom() failed");
    }

    if (sellerNumber == 5) {
        exit(0);
    }

    if (sellerNumber != 4) {
        printf("I received %d seller number\n", sellerNumber);

        for (int i = 0; i < clientsAmount; ++i) {
            if (clients[i].currentSeller <= 2 && clients[i].sellersToVisit[clients[i].currentSeller] == sellerNumber) {
                sleep(rand() % 3 + 1);
                while (clients[i].currentSeller <= 2 && clients[i].sellersToVisit[++clients[i].currentSeller] == 0);
            }
        }
    }

    if (sendto(sock, &sellerNumber, sizeof(sellerNumber), 0, (struct sockaddr *) &multicastAddr, sizeof(multicastAddr)) != sizeof(sellerNumber)) {
        DieWithError("send() failed");
    }
    printf("I sent %d seller number to several hosts\n", sellerNumber);

    if (!UnservedClients(clientsAmount)) {
        exit(0);
    }

    // close(clntSocket);
}
