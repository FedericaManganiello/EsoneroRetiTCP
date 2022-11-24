#ifdef WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#define BUFFERSIZE 1024
#define PROTOPORT 27015 // Numero di porta di default
#define LENGHT_TEXT 30
#define LENGHT_TEXT_PLUS 60

void ErrorHandler(char *errorMessage);
void ClearWinsock();
void ip_control (char *str1);
void port_control (char *str1);

struct stringhe{
    char string_1[LENGHT_TEXT];
	char string_2[LENGHT_TEXT];
	char string_3[LENGHT_TEXT_PLUS];
};

int main(){
	//inizializzazione dell'applicazione
	#ifdef WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);

	if (iResult != 0) {
		printf ("Error at WSASturtup\n");
		return -1;
	}
	#endif
	//creazione della socket
	int Csocket;
	Csocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(Csocket < 0){
		ErrorHandler("FAILED! Socket not created\n" );
		closesocket(Csocket);
		ClearWinsock();
		return -1;
	}

	//struttura sockaddr_in che identifica la socket settata all'ascolto
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;

	char *ipaddress = malloc(4*sizeof(char));
	char *port_number = malloc(4*sizeof(char));

	printf("Enter Server's IP address\n");
	scanf("%s", ipaddress);
	ip_control(ipaddress);


	printf("Enter Server's port number\n");
	scanf("%s", port_number);
	port_control(port_number);

	sad.sin_addr.s_addr = inet_addr(ipaddress);
	sad.sin_port = htons(atoi(port_number));

	/*La funzione connect accetta come parametri la socket client creata, la struttura della socket in ascolto
	e la lunghezza di quest'ultima*/
	if(connect(Csocket, (struct sockaddr*) &sad, sizeof(sad)) < 0){
		ErrorHandler("Connection failed\n");
		closesocket(Csocket);
		ClearWinsock();
		return -1;
	}

	int bytesRcvd;
    char buf[BUFFERSIZE];
    printf("Received: ");

	if((bytesRcvd = recv(Csocket, buf, BUFFERSIZE -1, 0)) <= 0){
		ErrorHandler("recv() failed or connection closed prematurely\n");
		closesocket(Csocket);
		ClearWinsock();
		return -1;
	}

	buf[bytesRcvd] = '\0';
	printf("%s", buf);


	char *string_1 = malloc(20*sizeof(char));
	char *string_2= malloc(20*sizeof(char));
	struct stringhe instance;

	while(!strcmp(instance.string_3, "Bye") == 0){


		printf("\nEnter the first string: ");
		scanf("%s", instance.string_1);
		printf("\nEnter the second string: ");
		scanf("%s", instance.string_2);


		if(send(Csocket,(void *)&instance, sizeof(struct stringhe), 0) <= 0){
			ErrorHandler("send() sent a different number of bytes than expected");
			closesocket(Csocket);
			ClearWinsock();
			return -1;
		}

		printf("\nStruct successfully sent...\n");

		if(recv(Csocket,(void *)&instance, sizeof(struct stringhe), 0) <= 0){
			ErrorHandler("send() sent a different number of bytes than expected");
			closesocket(Csocket);
			ClearWinsock();
			return -1;
		}

		printf("Concatenated string received from Server = %s\n", instance.string_3);
		system("pause");
	}

	printf("Closing connection...");
	closesocket(Csocket);
	ClearWinsock();

}

void ErrorHandler(char *errorMessage){
	printf("%s", errorMessage);
}

void ClearWinsock(){
	#if defined WIN32
	WSACleanup();
	#endif
}

void ip_control (char *str1){

	const char *str2 = "127.0.0.1";
	char *ipaddress_2 = malloc(4*sizeof(char));

	while( !strcmp(str1, str2) == 0){
		printf("Incorrect IP Addres, try again.\n");
		scanf("%s", ipaddress_2);
		strcpy(str1, ipaddress_2);

	}
}

void port_control (char *str1){

	const char *str2 = "27015";
	char *port_number_2 = malloc(4*sizeof(char));

	while( !strcmp(str1, str2) == 0){
		printf("Incorrect Port Number, try again.\n");
		scanf("%s", port_number_2);
		strcpy(str1, port_number_2);
	}
}

