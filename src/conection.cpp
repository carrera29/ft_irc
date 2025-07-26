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

    // struct addrinfo {
    //     int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    //     int              ai_family;    // AF_INET (IPv4), AF_INET6 (IPv6), AF_UNSPEC (whatever)
    //     int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    //     int              ai_protocol;  // use 0 for "any"
    //     socklen_t        ai_addrlen;   // size of ai_addr in bytes
    //     struct sockaddr *ai_addr;      // pointer to struct sockaddr_in or _in6
    //     char            *ai_canonname; // full canonical hostname
    //     struct addrinfo *ai_next;      // linked list, next node
    // };

    struct addrinfo hints;
    struct addrinfo *all, *i;
    // all es un puntero a una lista enlazada de estructuras addrinfo

    // memset(void *ptr, int value, size_t num);
    // memset es una función que llena un bloque de memoria con un valor específico
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // AF_UNSPEC permite IPv4 o IPv6
    hints.ai_socktype = SOCK_STREAM; // SOCK_STREAM indica que es un socket de tipo flujo (TCP)
    hints.ai_flags = AI_PASSIVE; // AI_PASSIVE indica que el socket se usará para aceptar conexiones entrantes

 
    // int getaddinfo(const char *hostname, const char *service, const struct addrinfo *hints, struct addrinfo **res);
    // prueba a obtener información de dirección para el servidor
    // NULL para el hostname significa que se vinculará a todas las interfaces
    // "8080" como número de puerto
    // hints es un puntero a la estructura creada anteriormente
    // res es un puntero a un puntero que se llenará con la lista enlazada de addrinfo
    if (getaddrinfo(NULL, "8080", &hints, &all) != 0) {
        std::cerr << "Failed to getaddrinfo" << std::endl;
        return 1;
    }

    // Pasar por la lista enlazada de addrinfo
    int serverSocket;
    for (i = all; i != NULL; i = i->ai_next) {
        // int socket(int domain, int type, int protocol);
        // Crea un socket usando la familia de direcciones, tipo de socket y protocolo
        // Un socket es un punto de comunicación para enviar y recibir datos
        serverSocket = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (serverSocket == -1) 
            continue;
        
        // int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
        // Asegura la reutilización de la dirección del socket e impide errores de "Dirección ya en uso"
        int yes = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) { 
            std::cerr << "Failed to setsockopt" << std::endl;
            return 1;
        }

        // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
        // Vincula el socket a la dirección y puerto especificados en la estructura addrinfo 
        // para que el socket pueda aceptar conexiones
        if (bind(serverSocket, i->ai_addr, i->ai_addrlen) == -1) {
            close(serverSocket);
            continue;
        }
        break;
    }

    // Libera la lista enlazada de addrinfo
    freeaddrinfo(all);

    if (i == NULL) {
        std::cerr << "Failed to bind" << std::endl;
        return 1;
    }

    // int listen(int sockfd, int backlog);
    // Comienza a escuchar conexiones entrantes en el socket del servidor
    // El parámetro backlog especifica el número máximo de conexiones pendientes
    int maxQueue = 10;
    if (listen(serverSocket, maxQueue) == -1) {
        std::cerr << "Failed to listen" << std::endl;
        return 1;
    }
    std::cout << "Server listening on port 8080" << std::endl;

    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    // Acepta una conexión entrante en el socket del servidor
    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == -1) {
        std::cerr << "Failed to accept" << std::endl;
        return 1;
    }
    std::cout << "Client connected" << std::endl;

    // int send(int sockfd, const void *buf, size_t len, int flags);
    // Para enviar un mensaje a cliente

    // int recv(int sockfd, void *buf, size_t len, int flags);
    // Recibe datos del socket del cliente

	char buffer[256];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    std::cout << "RAW MESSAGE: " << buffer << std::endl;


    close(clientSocket);
	close(serverSocket);

    return 0;
}
