#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define MAX_MESSAGE_LENGTH 1024
#define PORT 8080

// ����������� ������ ��� �������
typedef struct {
SOCKET socket;
} Client;

// ����������� ������ ��� �������
typedef struct {
SOCKET socket;
} Server;

// ������� ��� �������� ������ � �������� � ������
SOCKET create_socket(char type, int port);
struct sockaddr_in serverAddress, clientAddress;

void error(const char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main() {
	
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
    WSADATA wsaData;
    Server server;
    Client client;
    char message[MAX_MESSAGE_LENGTH];
    // ������������� Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error("������ ������������� Winsock");
    }
    
    printf("s - ������ | k - ������\n");
    char c;
    c = getchar();
	
	if(c == 's') 
	{
		// �������� ������
		server.socket = create_socket(c, PORT);
		if (server.socket == INVALID_SOCKET) {
			error("�� ������� ������� �����");
		}
		
		// �������� ������ � ������
		if (bind(server.socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			error("������ �������� ������");
		}
		
		// ������������� ������
		if (listen(server.socket, SOMAXCONN) == SOCKET_ERROR) {
			error("������ ��� ������������� ������");
		}
		
		printf("������ �������. �������� �����������...\n");
		
		// �������� �������� ����������
		int clientAddressSize = sizeof(clientAddress);
		client.socket = accept(server.socket, (struct sockaddr*)&clientAddress, &clientAddressSize);
		if (client.socket == INVALID_SOCKET) {
			error("������ ��� �������� ����������");
		}
		
		printf("����������� �������. ����� � ������ �����������.\n");
		
		while (1) {
			// ������� ������ ���������
			memset(message, 0, MAX_MESSAGE_LENGTH);
			
			// ��������� ��������� �� �������
			if (recv(client.socket, message, MAX_MESSAGE_LENGTH, 0) == SOCKET_ERROR) {
			error("������ ��� ��������� ���������");
			}
			
			printf("������: %s\n", message);
			
			// �������� ��������� ��������� �������
			printf("������ (������� ���������): ");
			fflush(stdin);
			fgets(message, MAX_MESSAGE_LENGTH, stdin);
			message[strcspn(message, "\n")] = '\0';
			
			if (send(client.socket, message, strlen(message), 0) == SOCKET_ERROR) {
			error("������ ��� �������� ���������");
			}
		}
		
		// �������� ������ � ������������� �������� Winsock
		closesocket(client.socket);
		closesocket(server.socket);
		WSACleanup();
	}
	else if(c == 'k') 
	{
		client.socket = create_socket(c, PORT);
		
		// �������� ������
	    if (client.socket == INVALID_SOCKET) {
	        error("�� ������� ������� �����");
	    }
	    	
	    // ����������� � �������
	    if (connect(client.socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
	        error("������ ��� ����������� � �������");
	    }
	
	    printf("����������� � ������� �����������. ����� � ������ �����������.\n");
	
	    while (1) {
			// �������� ��������� �������
			printf("������ (������� ���������): ");
			fgets(message, MAX_MESSAGE_LENGTH, stdin);
			message[strcspn(message, "\n")] = '\0';
			
			if (send(client.socket, message, strlen(message), 0) == SOCKET_ERROR) {
			error("������ ��� �������� ���������");
			}
			
			// ������� ������ ���������
			memset(message, 0, MAX_MESSAGE_LENGTH);
			
			// ��������� ��������� ��������� �� �������
			if (recv(client.socket, message, MAX_MESSAGE_LENGTH, 0) == SOCKET_ERROR) {
			error("������ ��� ��������� ���������");
			}
			
			printf("������: %s\n", message);
	    }
	    // �������� ������ � ������������� �������� Winsock
	    closesocket(client.socket);
	    WSACleanup();
	}
    return 0;
}

SOCKET create_socket(char type, int port) {
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	serverAddress.sin_family = AF_INET;
	if(type == 's') {
		serverAddress.sin_addr.s_addr = INADDR_ANY;
	} 
	else if(type == 'k') {
		printf("������� IP ����� �������: ");
		char ipserv[MAX_MESSAGE_LENGTH];
		getchar();
		fgets(ipserv, MAX_MESSAGE_LENGTH, stdin);
	    ipserv[strcspn(ipserv, "\n")] = '\0';
	    serverAddress.sin_addr.s_addr = inet_addr(ipserv);
	}
	serverAddress.sin_port = htons(port);	
	return s;
}

