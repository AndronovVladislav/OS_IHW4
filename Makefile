all: mark5 mark7

mark5:
	# gcc -g mark4-5/client.c Other/DieWithError.c -o mark4-5/client
	# gcc -g mark4-5/server.c Other/CreateTCPServerSocket.c Other/HandleTCPClient.c Other/DieWithError.c Other/utils.c -o mark4-5/server

mark7:
	gcc -g mark6-7/client.c Other/DieWithError.c -o mark6-7/client
	gcc -g mark6-7/server.c Other/DieWithError.c Other/CreateTCPServerSocket.c Other/HandleTCPClient.c Other/utils.c -o mark6-7/server
	gcc -g mark6-7/observer.c Other/DieWithError.c Other/utils.c -o mark6-7/observer

clear:
	rm mark4-5/client mark4-5/server mark6-7/client mark6-7/server mark6-7/observer
