#ifdef WIN32
#include <winsock.h>

//#pragma comment(lib,"ws_32.lib")
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h> // for atoi()
#define PROTOPORT 27015 // default protocol port number
#define QLEN 6 // size of request queue
#define BUFFERSIZE 1024
#define LENGHT_TEXT 30
#define LENGHT_TEXT_PLUS 60
void ErrorHandler(char *errorMessage);
void ClearWinsock();

struct stringhe{
	char string_1[LENGHT_TEXT];
	char string_2[LENGHT_TEXT];
	char string_3[LENGHT_TEXT_PLUS];
};

int main(int argc, char *argv[]) {


	int port;

	if (argc > 1) {
		port = atoi(argv[1]); // if argument specified convert argument to bianry
	}
	else
		port = PROTOPORT; // use default port number


	if (port < 0) {
		printf("Bad port number %s \n", argv[1]);
		return 0;
	}

	#ifdef WIN32 // initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

		if (iResult != 0) {
			ErrorHandler("Error at WSAStartup()\n");
			return 0;
		}

	#endif

	int MySocket;

	    struct sockaddr_in sad;

		sad.sin_family = AF_INET;
		sad.sin_addr.s_addr = inet_addr("127.0.0.1");
		sad.sin_port = htons(27015);
	MySocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);


	if (MySocket < 0){
		ErrorHandler("FAILED! Socket not created\n");
		return -1;
	}




	if(bind(MySocket, (struct sockaddr*) &sad, sizeof(sad)) < 0){
		ErrorHandler("bind() failed.\n");
		closesocket(MySocket);
		return -1;
	}

	int qlen = 6;

	if(listen(MySocket, qlen) < 0){
		ErrorHandler("listen() failed.\n");
		closesocket(MySocket);
		return -1;
	}

	struct sockaddr_in cad;
	int clientSocket;
	int clientLen;
	printf("Waiting for a client to connect...");

	while(1){

		clientLen = sizeof(cad);

		if((clientSocket = accept(MySocket, (struct sockaddr*) &cad, &clientLen)) <0){
			ErrorHandler("listen() failed.\n");
			closesocket(MySocket);
			ClearWinsock();
			return 0;
		}

		printf("\nHandling client %s\n", inet_ntoa(cad.sin_addr));

		char* inputString = "Successful connection\nSend a struct with two strings:\n\n";
		int stringLen = strlen(inputString);

		if(send(clientSocket, inputString, stringLen, 0) != stringLen){
			ErrorHandler("send() sent a different number of bytes than expected");
			closesocket(clientSocket);
			ClearWinsock();
			return -1;
		}

		while(1){

			printf("\nReceived: ");
			struct stringhe instance;

			if(recv(clientSocket,(void *)&instance, sizeof(struct stringhe), 0) <= 0){
				ErrorHandler("send() sent a different number of bytes than expected");
				closesocket(clientSocket);
				ClearWinsock();
				return -1;
			}

			printf("%s, %s", instance.string_1, instance.string_2);

			const char *quit = "quit";


			if(strcmp(instance.string_1, quit) == 0 || strcmp(instance.string_2, quit) == 0){

				char* bye = "Bye";
				strcpy(instance.string_3, bye);
				printf("\nExamining the request to close the connection...");
			}
			else{

				strcpy(instance.string_3, strcat(instance.string_1, instance.string_2) );
				printf("\nConcatenated string = %s", instance.string_3);

			}

			if(send(clientSocket,(void *)&instance, sizeof(struct stringhe), 0) <= 0){
					ErrorHandler("send() sent a different number of bytes than expected");
					closesocket(clientSocket);
					ClearWinsock();
					return -1;
			}
		}
	}
}

void ErrorHandler(char *errorMessage){

	printf("%s", errorMessage);
}

void ClearWinsock(){

	#if defined WIN32
	WSACleanup();
	#endif
}
