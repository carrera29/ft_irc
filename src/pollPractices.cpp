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

    struct addrinfo hints;
    struct addrinfo *all, *i;

    /*
        struct addrinfo {
            int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
            int              ai_family;    // AF_INET (IPv4), AF_INET6 (IPv6), AF_UNSPEC (whatever)
            int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
            int              ai_protocol;  // use 0 for "any"
            size_t           ai_addrlen;   // size of ai_addr in bytes
            struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
            char            *ai_canonname; // full canonical hostname

            struct addrinfo *ai_next;      // linked list, next node
        }; 
    */

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
        if (bind(serverSocket, i->ai_addr, i->ai_addrlen) == -1) {
            close(serverSocket);
            continue;
        }
        break;
    }
    freeaddrinfo(all);

    int maxQueue = 10;
    if (listen(serverSocket, maxQueue) == -1) {
        std::cerr << "Failed to listen" << std::endl;
        return 1;
    }
    std::cout << "Server listening on port 8080..." << std::endl;

    
    // creamos un vector de pollfd para almacenar los descriptores de los sockets
    // e inicializamos el vector con el descriptor del socket del servidor

	/*
        struct pollfd {
            int fd;         // el fd del socket
            short events;   // bitmap son los eventos que queremos monitorizar
            short revents;  // son los eventos que han ocurrido
        };
    */

    std::vector<struct pollfd> fds;
	std::vector<client> clients;
	struct pollfd server;


	memset(&server, 0, sizeof(server));
	server.fd = serverSocket;
	server.events = POLLIN;
	server.revents = 0;
	
    fds.push_back(server);


    while (true) {
        int events = poll(fds.data(), fds.size(), -1);
        if (events == -1) {
            std::cerr << "Failed to poll" << std::endl;
            return 1;
        }
        for (int i = 0; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {

				if (fds[i].fd == serverSocket) {
					int clientSocket = accept(serverSocket, NULL, NULL); //accept(serverSocket, &clientAddr, &clientAddrLen);
					if (clientSocket == -1) {
						std::cerr << "Failed to accept" << std::endl;
						return 1;
					}
					
					// client newClient(clientSocket, i);
					// clients.push_back(newClient);
					struct pollfd newFd;
					newFd.fd = clientSocket;
					newFd.events = POLLIN;
					newFd.revents = 0;
					fds.push_back(newFd);

					int bytes_sent = send(clientSocket, "Welcome to the server\n", strlen("Welcome to the server\n"), 0);
					if (bytes_sent == -1) {
						std::cerr << "Failed to send" << std::endl;
						return 1;
					}
				}
				else {
					char buffer [1024] = {0};
					int bytesRead = recv(
						fds[i].fd, // el fd del socket del cliente que ha enviado datos
						buffer, // el buffer donde se almacenarán los datos
						sizeof(buffer) - 1, // el tamaño del buffer
						0); // flags

					if (bytesRead <= 0) {
						std::cout << "Client disconnected" << std::endl;
						close(fds[i].fd);
						fds.erase(fds.begin() + i);
						--i;
					}
					else {
						buffer[bytesRead] = '\0';
						std::cout << "Cliente nº " << i << ": " <<buffer << std::endl;
						memset(buffer, 0, sizeof(buffer));
					}
        		}	
    		}
		}
	}

	close(serverSocket);

    return 0;
}
