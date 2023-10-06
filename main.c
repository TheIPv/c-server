#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define MAX_MESSAGE_LENGTH 1024
#define PORT 8080

// определение класса для клиента
typedef struct {
SOCKET socket;
} Client;

// определение класса для сервера
typedef struct {
SOCKET socket;
} Server;

// функция для создания сокета и привязки к адресу
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
    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error("Ошибка инициализации Winsock");
    }
    
    printf("s - сервер | k - клиент\n");
    char c;
    c = getchar();
	
	if(c == 's') 
	{
		// Создание сокета
		server.socket = create_socket(c, PORT);
		if (server.socket == INVALID_SOCKET) {
			error("Не удалось создать сокет");
		}
		
		// Привязка сокета к адресу
		if (bind(server.socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			error("Ошибка привязки сокета");
		}
		
		// Прослушивание сокета
		if (listen(server.socket, SOMAXCONN) == SOCKET_ERROR) {
			error("Ошибка при прослушивании сокета");
		}
		
		printf("Сервер запущен. Ожидание подключений...\n");
		
		// Принятие входящих соединений
		int clientAddressSize = sizeof(clientAddress);
		client.socket = accept(server.socket, (struct sockaddr*)&clientAddress, &clientAddressSize);
		if (client.socket == INVALID_SOCKET) {
			error("Ошибка при принятии соединения");
		}
		
		printf("Подключение принято. Готов к обмену сообщениями.\n");
		
		while (1) {
			// Очистка буфера сообщения
			memset(message, 0, MAX_MESSAGE_LENGTH);
			
			// Получение сообщения от клиента
			if (recv(client.socket, message, MAX_MESSAGE_LENGTH, 0) == SOCKET_ERROR) {
			error("Ошибка при получении сообщения");
			}
			
			printf("Клиент: %s\n", message);
			
			// Отправка ответного сообщения клиенту
			printf("Сервер (Введите сообщение): ");
			fflush(stdin);
			fgets(message, MAX_MESSAGE_LENGTH, stdin);
			message[strcspn(message, "\n")] = '\0';
			
			if (send(client.socket, message, strlen(message), 0) == SOCKET_ERROR) {
			error("Ошибка при отправке сообщения");
			}
		}
		
		// Закрытие сокета и высвобождение ресурсов Winsock
		closesocket(client.socket);
		closesocket(server.socket);
		WSACleanup();
	}
	else if(c == 'k') 
	{
		client.socket = create_socket(c, PORT);
		
		// Создание сокета
	    if (client.socket == INVALID_SOCKET) {
	        error("Не удалось создать сокет");
	    }
	    	
	    // Подключение к серверу
	    if (connect(client.socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
	        error("Ошибка при подключении к серверу");
	    }
	
	    printf("Подключение к серверу установлено. Готов к обмену сообщениями.\n");
	
	    while (1) {
			// Отправка сообщения серверу
			printf("Клиент (Введите сообщение): ");
			fgets(message, MAX_MESSAGE_LENGTH, stdin);
			message[strcspn(message, "\n")] = '\0';
			
			if (send(client.socket, message, strlen(message), 0) == SOCKET_ERROR) {
			error("Ошибка при отправке сообщения");
			}
			
			// Очистка буфера сообщения
			memset(message, 0, MAX_MESSAGE_LENGTH);
			
			// Получение ответного сообщения от сервера
			if (recv(client.socket, message, MAX_MESSAGE_LENGTH, 0) == SOCKET_ERROR) {
			error("Ошибка при получении сообщения");
			}
			
			printf("Сервер: %s\n", message);
	    }
	    // Закрытие сокета и высвобождение ресурсов Winsock
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
		printf("Введите IP адрес сервера: ");
		char ipserv[MAX_MESSAGE_LENGTH];
		getchar();
		fgets(ipserv, MAX_MESSAGE_LENGTH, stdin);
	    ipserv[strcspn(ipserv, "\n")] = '\0';
	    serverAddress.sin_addr.s_addr = inet_addr(ipserv);
	}
	serverAddress.sin_port = htons(port);	
	return s;
}

