#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <vector>
#include "../include/client.hpp"

int main() {

    int             port = 8080; // puede ser entre 0 y 65535, menos los puertos reservados por el sistema operativo (0-1023)
    struct addrinfo hints;
    struct addrinfo *all, *i;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, "8080", &hints, &all);

    int serverSocket;
    for (i = all; i != NULL; i = i->ai_next) {
        serverSocket = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (serverSocket == -1)
            continue;

        int yes = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) { 
            std::cerr << "Failed to setsockopt" << std::endl;
            return 1;
        }

        if (bind(serverSocket, i->ai_addr, i->ai_addrlen) == -1) {
            close(serverSocket);
            continue;
        }
        break;
    }
    freeaddrinfo(all);

    if (i == NULL) {
        std::cerr << "Failed to bind" << std::endl;
        return 1;
    }

    int maxQueue = 10;
    if (listen(serverSocket, maxQueue) == -1) {
        std::cerr << "Failed to listen" << std::endl;
        return 1;
    }
    std::cout << "Server listening on port " << port << std::endl;

    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == -1) {
        std::cerr << "Failed to accept" << std::endl;
        return 1;
    }
    std::string welcome = ":server 001 ClaudiaTest :Welcome to My IRC Server\r\n";
    send(clientSocket, welcome.c_str(), welcome.size(), 0);

	char buffer[256];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    std::cout << "RAW MESSAGE: " << buffer << std::endl;

    close(clientSocket);
	close(serverSocket);

    return 0;
}
