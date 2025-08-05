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
            while ((i = command.find(";")) != (int)std::string::npos) {
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

    // getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
    // node: es el nombre del host o NULL para usar la dirección local, también puede ser una dirección IP ("127.0.0.1")
    // service: es el nombre del servicio o el número de puerto ("8080")
    // hints: es un puntero a una estructura addrinfo (AF_INET, SOCK_STREAM, AI_PASSIVE)
    // res: es un puntero a una lista de estructuras addrinfo que se rellenará con los resultados
    
    struct addrinfo *all, *i;
    getaddrinfo(NULL, "8080", IRCServer.getHints(), &all);

    int serverSocket;
    for (i = all; i != NULL; i = i->ai_next) {
        // socket(int domain, int type, int protocol)
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1)
            continue;

        int yes = 1; // para permitir reutilizar la dirección del socket
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) { 
            std::cerr << "Failed to setsockopt" << std::endl;
            return 1;
        }
        
        // bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
        // sockfd: es el descriptor del socket que se va a vincular
        // addr: es un puntero a una estructura sockaddr que contiene la dirección local
        // addrlen: es el tamaño de la estructura sockaddr
        // En este caso, i->ai_addr es un puntero a una estructura sockaddr que
        // contiene la dirección local y i->ai_addrlen es el tamaño de esa estructura.
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

    // La estructura pollfd se usa para monitorizar múltiples sockets
    // y detectar eventos como la llegada de nuevos datos o conexiones
	/*
        struct pollfd {
            int fd;         // el fd del socket
            short events;   // Eventos a monitorizar:
                            // POLLIN (entrada disponible), POLLOUT (salida disponible),
                            // POLLHUP (socket cerrado), POLLERR (error), POLLNVAL (fd no válido)
            short revents;  // Eventos que han ocurrido
        };
    */

    struct pollfd stdinfd;
    memset(&stdinfd, 0, sizeof(stdinfd));
    stdinfd.fd = 0;
    stdinfd.events = POLLIN; 
    stdinfd.revents = 0; 
    IRCServer.fds.push_back(stdinfd);

    struct pollfd server;
	memset(&server, 0, sizeof(server));
	server.fd = serverSocket;
	server.events = POLLIN;
	server.revents = 0;
    IRCServer.fds.push_back(server);
	

    while (true) {
        // poll(struct pollfd *fds, nfds_t nfds, int timeout)
        // fds: es un puntero a un array de estructuras pollfd que se van a monitorizar
        // nfds: es el número de estructuras pollfd en el array
        // timeout: es el tiempo máximo de espera en milisegundos, -1 para esperar indefinidamente
        int events = poll(IRCServer.fds.data(), IRCServer.sizeoffds(), -1);
        if (events == -1) {
            std::cerr << "Failed to poll" << std::endl;
            return 1;
        }
        for (size_t i = 0; i < IRCServer.sizeoffds(); ++i) {

            // Si hay eventos en el socket del servidor y es un evento de entrada (POLLIN)
            if (IRCServer.fds[i].revents & POLLIN) {
                if (IRCServer.fds[i].fd == 0) { // si la entrada es del stdin
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    int bytesRead = read(0, buffer, sizeof(buffer) - 1);
                    if (bytesRead > 0 && std::string(buffer) == "exit\n") {
                        std::cout << "Exiting server..." << std::endl;
                        goto exit_loop;
                    }
                }
                // Si el evento es del socket del servidor 
				if (IRCServer.fds[i].fd == serverSocket) { 
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
                    
                    
                    // char clientBuffer[1024] = "Please, sent your nickename and username for the connection\n";
                    // if (send(clientSocket, clientBuffer, strlen(clientBuffer), 0) == -1) {
					// 	std::cerr << "Failed to send" << std::endl;
					// 	return 1;
					// }
                    
                    std::string nickname = "client";
                    std::stringstream ss;
                    ss << i;
                    std::string username = ss.str();

                    client* newClient = new client(nickname, username, clientSocket);
                    IRCServer.clients[clientSocket] = newClient;
                    std::cout << "Client number " << i << " connected" << std::endl;

                    
					if (send(clientSocket, "Welcome to the server\n", strlen("Welcome to the server\n"), 0) == -1) {
                        std::cerr << "Failed to send welcome message" << std::endl;
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
    exit_loop:;
	close(serverSocket);

    return 0;
}
