all: mark5 mark7

mark5:
	gcc -g mark4-5/client.c Other/DieWithError.c -o mark4-5/client -lrt -lpthread
	gcc -g mark4-5/server.c Other/CreateUDPServerSocket.c Other/HandleTCPClient.c Other/DieWithError.c Other/utils.c -o mark4-5/server -lrt -lpthread

mark7:
	gcc -g mark6-7/client.c Other/DieWithError.c -o mark6-7/client -lrt -lpthread
	gcc -g mark6-7/server.c Other/DieWithError.c Other/CreateUDPServerSocket.c Other/HandleTCPClient.c Other/utils.c -o mark6-7/server -lrt -lpthread
	gcc -g mark6-7/observer.c Other/DieWithError.c Other/utils.c -o mark6-7/observer -lrt -lpthread

clear:
	rm mark4-5/client mark4-5/server mark6-7/client mark6-7/server mark6-7/observer