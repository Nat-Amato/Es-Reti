#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFERSIZE 512
#define PROTOPORT 50001 // Numero di porta di default
void errorhandler(char *error_message) {
	printf("%s",error_message);
}
void clearwinsock() {
	#if defined WIN32
	WSACleanup();
	#endif
}

int main(void) {
	#if defined WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2 ,2), &wsa_data);
	if (result != 0) {
		printf ("error at WSASturtup\n");
		return -1;
	}
	#endif
	// CREAZIONE DELLA SOCKET
	int c_socket;
	c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (c_socket < 0) {
		errorhandler("socket creation failed.\n");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}
	// COSTRUZIONE DELL’INDIRIZZO DEL SERVER
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP del server
	sad.sin_port = htons(50000); // Server port
	// CONNESSIONE AL SERVER
	if (connect(c_socket, (struct sockaddr *)&sad, sizeof(sad))< 0){
		errorhandler( "Failed to connect.\n" );
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}
	printf("Connessione Avvenuta. \n");

	char input_string[BUFFERSIZE] = "prova"; // Stringa da inviare
	printf("Inserire prima stringa: ");
	scanf("%s", input_string);
	int string_len = strlen(input_string); // Determina la lunghezza
	input_string[string_len] = '\0';
	string_len++;
	fflush(stdin);
	// INVIARE DATI AL SERVER
	if (send(c_socket, input_string, string_len, 0) != string_len){
		errorhandler("send() sent a different number of bytes than expected");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}

	printf("Inserire seconda stringa: ");
	scanf("%s", input_string);
	fflush(stdin);
	string_len = strlen(input_string);
	input_string[string_len] = '\0';
	string_len++;
	if (send(c_socket, input_string, string_len, 0) != string_len){
			errorhandler("send() sent a different number of bytes than expected");
			closesocket(c_socket);
			clearwinsock();
			return -1;
		}
	// RICEVERE DATI DAL SERVER
	int bytes_rcvd = 0;
	char buf[BUFFERSIZE]; // buffer for data from the server
	printf("Received: "); // Setup to print the echoed string

	/*
	if ((bytes_rcvd = recv(c_socket, buf, BUFFERSIZE - 1, 0)) <= 0){
		errorhandler("recv() failed or connection closed prematurely");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}*/


	recv(c_socket, buf, BUFFERSIZE - 1, 0);
	bytes_rcvd = strlen(buf);
	buf[bytes_rcvd] = '\0'; // Add \0 so printf knows where to stop
	printf("%s ", buf); // Print the echo buffer

	recv(c_socket, buf, BUFFERSIZE - 1, 0);
	bytes_rcvd = strlen(buf);
	buf[bytes_rcvd] = '\0'; // Add \0 so printf knows where to stop
	printf("%s ", buf); // Print the echo buffer


	// CHIUSURA DELLA CONNESSIONE
	closesocket(c_socket);
	clearwinsock();
	printf("\n"); // Print a final linefeed
	system("pause");
	return 0;
}
