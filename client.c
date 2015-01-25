#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

int StopMusic();
int PlayMusic(LPCWSTR NameOfFile);

int main(int argc, char *argv[])
{
	char *ip = "127.0.0.1";
	WSADATA wsaData; //Никому не нужная переменная
	int iResult;
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		printf("WSAStartup failed\n");
		return 1;
	}

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	if (getaddrinfo(ip, DEFAULT_PORT, &hints, &result)) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;

	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Connect to server.
	if (connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	int recvbuflen = DEFAULT_BUFLEN;
	char *sendbuf = "ololololololololololololololo";
	char recvbuf[DEFAULT_BUFLEN];

	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	if (shutdown(ConnectSocket, SD_SEND) == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive data until the server closes the connection
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			PlayMusic(L"test.wav");
			printf("Bytes received: %d\n", iResult);
			printf(recvbuf);
			printf("\n");
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (iResult > 0);

	// shutdown the send half of the connection since no more data will be sent
	if (shutdown(ConnectSocket, SD_SEND) == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	getchar();
	return 0;
}

int PlayMusic(LPCWSTR NameOfFile)
{
	if (!PlaySound(NameOfFile, 0, SND_ASYNC))// SND_ASYNC - продолжать выполнение
	{
		printf("Error occured!");
		return -1;
	}
	return 0;
}

int StopMusic()
{
	PlaySound(0, 0, 0);
	return 0;
}