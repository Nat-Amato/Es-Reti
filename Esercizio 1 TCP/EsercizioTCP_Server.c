#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h> // for atoi()
#define PROTOPORT 50000 // default protocol port number
#define QLEN 6 // size of request queue
void errorhandler(char *errorMessage) {
	printf ("%s", errorMessage);
}
void clearwinsock() {
	#if defined WIN32
	WSACleanup();
	#endif
}

int main(int argc, char *argv[]) {
	int port;
	if (argc > 1) {
		port = atoi(argv[1]); // if argument specified convert argument to binary
	}
	else
		port = PROTOPORT; // use default port number
	if (port < 0) {
		printf("bad port number %s \n", argv[1]);
		return 0;
	}
	#if defined WIN32 // initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != 0) {
		errorhandler("Error at WSAStartup()\n");
		return 0;
	}
	#endif
	// CREAZIONE DELLA SOCKET
	int my_socket;
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (my_socket < 0) {
		errorhandler("socket creation failed.\n");
		clearwinsock();
		return -1;
	}
	// ASSEGNAZIONE DI UN INDIRIZZO ALLA SOCKET
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad)); // ensures that extra bytes contain 0
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");
	sad.sin_port = htons(port); /* converts values between the host and
	network byte order. Specifically, htons() converts 16-bit quantities
	from host byte order to network byte order. */
	if (bind(my_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
		errorhandler("bind() failed.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}
	// SETTAGGIO DELLA SOCKET ALL'ASCOLTO
	if (listen (my_socket, QLEN) < 0) {
		errorhandler("listen() failed.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}
	// ACCETTARE UNA NUOVA CONNESSIONE
	struct sockaddr_in cad; // structure for the client address
	int client_socket; // socket descriptor for the client
	int client_len; // the size of the client address
	printf("Waiting for a client to connect...");
	while (1) { /* oppure for (;;) */
		client_len = sizeof(cad); // set the size of the client address
		if ((client_socket = accept(my_socket, (struct sockaddr*)&cad, &client_len)) < 0) {
			errorhandler("accept() failed.\n");
			// CHIUSURA DELLA CONNESSIONE
			closesocket(client_socket);
			clearwinsock();
			return 0;
		}
		printf("Handling client %s\n", inet_ntoa(cad.sin_addr));
		char buf[1000];
		char stringRecv1[1000];
		char stringRecv2[1000];
		recv(client_socket, buf, 100, 0);
		strcpy(stringRecv1, buf);
		printf("%s ", stringRecv1);

		for(int i = 0; stringRecv1[i] != '\0'; i++){
			stringRecv1[i] = toupper(stringRecv1[i]);
		}

		recv(client_socket, buf, 100, 0);
		strcpy(stringRecv2, buf);
		printf("%s \n", stringRecv2);

		for(int i = 0; stringRecv2[i] != '\0'; i++){
			stringRecv2[i] = tolower(stringRecv2[i]);
		}

		send(client_socket, stringRecv1, 100, 0);
		send(client_socket, stringRecv2, 100, 0);

	} // end-while
}
