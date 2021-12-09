#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h> /* for memset() */
#define ECHOMAX 255
#define PORT 48000
void ErrorHandler(char *errorMessage) {
	printf(errorMessage);
}
void ClearWinSock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main() {
#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("error at WSASturtup\n");
		return EXIT_FAILURE;
	}
#endif
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;
	unsigned int fromSize;
	char echoString[ECHOMAX];
	char echoBuffer[ECHOMAX];
	int echoStringLen;
	int respStringLen;
	printf("Inserisci la stringa echo da inviare al server: ");
	scanf("%s", echoString);
	if ((echoStringLen = strlen(echoString)) > ECHOMAX){
		ErrorHandler("echo word too long");
		system("pause");
		exit(EXIT_FAILURE);
	}

// CREAZIONE DELLA SOCKET
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		ErrorHandler("socket() failed");
		system("pause");
		exit(EXIT_FAILURE);
	}
// COSTRUZIONE DELL'INDIRIZZO DEL SERVER
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = PF_INET;
	printf("Inserire DNS e numero di porta: ");
	char addr[500];
	fflush(stdin);
	gets(addr);
	char * token = strtok(addr, ":"); // separazione della stringa dandoci DNS
// CONVERSIONE DA DNS A IP
	struct hostent *host;
	host = gethostbyname(token);
	if(host == NULL){
		fprintf(stderr, "gethostbyname() failed.\n");
		system("pause");
		exit(EXIT_FAILURE);
	} else {
		struct in_addr* ina = (struct in_addr*) host->h_addr_list[0];
		char * ip = inet_ntoa(*ina);
		echoServAddr.sin_addr.s_addr = inet_addr(ip);
	}
	for(int i = 0; i == 0; i++) { // separazione della stringa dandoci la porta
		token = strtok(NULL, ":");
		echoServAddr.sin_port = htons(atoi(token));
	}
// INVIO DELLA STRINGA ECHO AL SERVER
	if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) != echoStringLen){
		ErrorHandler("sendto() sent different number of bytes than expected");
		system("pause");
		exit(EXIT_FAILURE);
	}
// RITORNO DELLA STRINGA ECHO
			fromSize = sizeof(fromAddr);
			respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr*) &fromAddr, &fromSize);
			if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
				fprintf(stderr, "Error: received a packet from unknown source.\n");
				system("pause");
				exit(EXIT_FAILURE);
			}
			echoBuffer[respStringLen] = '\0'; // inutile con memset
printf("Received: %s\n", echoBuffer);
			closesocket(sock);
			ClearWinSock();
			system("pause");
			return EXIT_SUCCESS;
		}
