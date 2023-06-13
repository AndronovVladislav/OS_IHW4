#include "utils.h"

void PrintClients(int clientsAmount) {
    int shmid;
    if ((shmid = shm_open(QUEUE_REGION, O_RDWR, S_IRWXU)) == -1) {
        perror("shm_open");
        exit(-1);
    }

    client* clients = mmap(0, sizeof(client) * clientsAmount, PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);

    for (int i = 0; i < clientsAmount; ++i) {
        // printf("%d ", clients[i].currentSeller);
        for (int j = 0; j < SELLERS_AMOUNT; ++j) {
            if (j == clients[i].currentSeller) {
                PRINT_GREEN(clients[i].sellersToVisit[j]);
            } else {
                printf("%d ", clients[i].sellersToVisit[j]);
            }
        }
        printf("\n");
    }
}

void CreateClients(int clientsAmount) {
    int shmid;
    if ((shmid = shm_open(QUEUE_REGION, O_CREAT | O_RDWR, S_IRWXU)) == -1) {
        perror("shm_open");
        exit(-1);
    }

    if (ftruncate(shmid, sizeof(client) * clientsAmount) == -1) {
        perror("ftruncate");
        exit(-1);
    }

    client* clients = mmap(0, sizeof(client) * clientsAmount, PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);

    for (int i = 0; i < clientsAmount; ++i) {
        srand(time(NULL));
        int busy[4] = {0, 0, 0, 0};

        for (int j = 0; j < SELLERS_AMOUNT; ++j) {
            while (1) {
                int potentialSeller = (rand() % 4);

                if (!busy[potentialSeller]) {
                    clients[i].sellersToVisit[j] = potentialSeller;
                    busy[potentialSeller] = 1;
                    break;
                }
            }
        }

        for (int j = 0; j < SELLERS_AMOUNT; ++j) {
            if (clients[i].sellersToVisit[j]) {
                clients[i].currentSeller = j;
                break;
            }
        }

        sleep(1);
    }
}

int UnservedClients(int clientsAmount) {
    int shmid;
    if ((shmid = shm_open(QUEUE_REGION, O_RDWR, S_IRWXU)) == -1) {
        perror("shm_open");
        exit(-1);
    }

    client* clients = mmap(0, sizeof(client) * clientsAmount, PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);

    for (int i = 0; i < clientsAmount; ++i) {
        if (clients[i].currentSeller != 3) {
            return 1;
        }
    }

    return 0;
}