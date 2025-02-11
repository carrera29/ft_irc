#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <vector>
#include <sstream>
#include "../include/client.hpp"
#include "../include/server.hpp"


void   commandParser(std::string msg) {

    /*
        message         ::= ['@' <tags> SPACE] [':' <source> SPACE] <command> <parameters> <crlf>
        SPACE           ::=  %x20 *( %x20 )   ; space character(s)
        crlf            ::=  %x0D %x0A        ; "carriage return" "linefeed"
        
        The specific parts of an IRC message are:
            tags: Optional metadata on a message, starting with ('@', 0x40).
            source: Optional note of where the message came from, starting with (':', 0x3A).
            command: The specific command this message represents.
            parameters: If it exists, data relevant to this specific command.
    */

    std::stringstream   ss(msg);
    std::string         command;
    int                 nComm = 0;

    while (ss) {

        std::string command;
        ss >> command;

        if (nComm == 0 && command[0] == '@') {
            std::string tag = command.substr(1, command.length());
            std::vector<std::string> AllTags;
            
            int i = 0;
            while ((i = command.find(";")) != std::string::npos) {
                AllTags.push_back(command.substr(0, i));
                command.erase(0, i + 1);
            }
            AllTags.push_back(command);

            for (size_t i = 0; i < AllTags.size(); i++)
                std::cout << "Tag " << i + 1 << ": " << AllTags[i] << std::endl;
            nComm++;
        }
        else if (nComm < 2 && command[0] == ':') {
            // https://modern.ircdocs.horse/#client-messages
            std::string source = command.substr(1, command.length());
            std::cout << "Source: " << source << std::endl;
            nComm += 2;
        }
        else if (nComm > 1) {

            /*
                Specific to channel operators:
                    if (word == "KICK") {}
                    else if (word == "INVITE") {}
                    else if (word == "TOPIC") {}
                    else if (word == "MODE") {}
            */

            if (command[0] == ':') {
                std::string msg = command.substr(1, command.length());
                std::cout << "Msg: " << msg << std::endl;
            }
        }
        else {
            std::cout << "Error: format msg" << std::endl;
            break;
        }
    }
}

int main(void) {

    Server IRCServer("password", 8080);

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

    IRCServer.setAddressInfo();

    struct addrinfo *all, *i;
    getaddrinfo(NULL, "8080", IRCServer.getHints(), &all);

    int serverSocket;
    for (i = all; i != NULL; i = i->ai_next) {
        serverSocket = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (serverSocket == -1)
            continue;

        // reutilizar el puerto si está ocupado por otro proceso que no ha cerrado el socket
        // SOL_SOCKET es el nivel de socket, SO_REUSEADDR es la opción que permite reutilizar el puerto
        int yes = 1;   // un puntero a int con valor 1 para activar la opción
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) { 
            std::cerr << "Failed to setsockopt" << std::endl;
            return 1;
        }
        // enlazar el socket a la dirección y puerto
        if (bind(serverSocket, i->ai_addr, i->ai_addrlen) == -1) {
            close(serverSocket);
            continue;
        }
        IRCServer.setSocket(serverSocket);
        break;
    }
    freeaddrinfo(all);

    if (i == NULL) {
        std::cerr << "Failed to bind" << std::endl;
        return 1;
    }

    int maxQueue = 10;
    if (listen(IRCServer.getSocket(), maxQueue) == -1) { // maxQueue es el número máximo de conexiones pendientes
        std::cerr << "Failed to listen" << std::endl;
        return 1;
    }
    std::cout << "Server listening on port 8080..." << std::endl;

	/*
        struct pollfd {
            int fd;         // el fd del socket
            short events;   // bitmap son los eventos que queremos monitorizar
            short revents;  // son los eventos que han ocurrido
        };
    */

	struct pollfd server;
	memset(&server, 0, sizeof(server));
	server.fd = serverSocket;
	server.events = POLLIN;
	server.revents = 0;
	
    IRCServer.fds.push_back(server);

    while (true) {
        int events = poll(IRCServer.fds.data(), IRCServer.sizeoffds(), -1);
        if (events == -1) {
            std::cerr << "Failed to poll" << std::endl;
            return 1;
        }
        for (int i = 0; i < IRCServer.sizeoffds(); ++i) {

            if (IRCServer.fds[i].revents & POLLIN) { // si hay datos para leer

				if (IRCServer.fds[i].fd == serverSocket) { // nueva conexión
                   
                    // sockaddr_in clientAddr = {}; // almacena información del cliente (IP y puerto) 
                    // socklen_t   size = sizeof(clientAddr); // accept() la actualiza con el tamaño real de la dirección
					int clientSocket = accept(serverSocket, NULL, NULL); //accept(serverSocket, &clientAddr, &clientAddrLen);
					if (clientSocket == -1) {
						std::cerr << "Failed to accept" << std::endl;
						return 1;
					}

                    struct pollfd newfd;
                    memset(&newfd, 0, sizeof(newfd));
                    newfd.fd = clientSocket;
                    newfd.events = POLLIN;
                    newfd.revents = 0;
                    IRCServer.fds.push_back(newfd);
                
                    client* newClient = new client(clientSocket);
                    IRCServer.clients[clientSocket] = newClient;
                    std::cout << "Client number " << i << " connected" << std::endl;

					int bytes_sent = send(clientSocket, "Welcome to the server\n", strlen("Welcome to the server\n"), 0);
					if (bytes_sent == -1) {
						std::cerr << "Failed to send" << std::endl;
						return 1;
					}
				}
				else {
					char buffer [1024] = {0};
                    memset(buffer, 0, sizeof(buffer));
					int bytesRead = recv(IRCServer.fds[i].fd, buffer, sizeof(buffer) - 1, 0);

					if (bytesRead <= 0) {
						
                        for (IRCServer.it = IRCServer.clients.begin(); IRCServer.it != IRCServer.clients.end(); ++IRCServer.it) {
                            if (IRCServer.it->first == IRCServer.fds[i].fd) {
                                delete IRCServer.it->second;
                                IRCServer.it = IRCServer.clients.erase(IRCServer.it);
                                close(IRCServer.fds[i].fd);
						        IRCServer.fds.erase(IRCServer.fds.begin() + i);
                                std::cout << "Client number " << i << " disconnected" << std::endl;
                                break;
                            }
                        }
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
