/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 22:04:03 by pollo             #+#    #+#             */
/*   Updated: 2025/02/03 21:55:53 by pollo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>


int main()
{
    int				serverSocket;
    struct addrinfo	hints;
    struct addrinfo	*all, *i;
    
	// Establecemos los criterios de búsqueda de direcciones de red
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

	// Buscamos las direcciones de red que cumplan con los criterios establecidos en hints
    getaddrinfo(NULL, "8080", &hints, &all);

	// Iteramos sobre las direcciones de red encontradas que cumplan con los criterios
	// usamos bind() para asociar una dirección de red a un socket y un puerto
    for (i = all; i != NULL; i = i->ai_next)
    {
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

    if (i == NULL) {
        std::cerr << "Failed to bind" << std::endl;
        return 1;
    }

	// Ponemos el socket en modo de escucha
    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Failed to listen" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port 8080... Press 'q' to stop." << std::endl;

	// Aceptamos la conexión de un cliente
	sockaddr_storage clientAddr;
	socklen_t clientAddrSize = sizeof(clientAddr);
	int clientfd = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
	if (clientfd == -1) {
		std::cerr << "Failed to accept" << std::endl;
		return 1;
	}

	// Enviamos un mensaje al cliente
	const char* msg = "Hello, client!\n";
    int len = strlen(msg);
	
    int bytes_sent = send(clientfd, msg, len, 0);
	if (bytes_sent == -1) {
		std::cerr << "Failed to send" << std::endl;
		return 1;
	}

	// Configuramos pollfd para el cliente y la entrada estándar
	struct pollfd fds[2];
	fds[0].fd = clientfd; // Socket del cliente
	fds[0].events = POLLIN; // Espera a que haya datos para leer
	fds[1].fd = STDIN_FILENO; // Entrada estándar por teclado
	fds[1].events = POLLIN; // Espera a que haya datos para leer



	while (true) {

		int ret = poll(fds, 2, -1); // fds, nfds, timeout (-1 para esperar indefinidamente)
		if (ret == -1) {
			std::cerr << "Failed to poll" << std::endl;
			return 1;
		}

		// Si fd de cliente tiene datos para leer (POLLIN)
		if (fds[0].revents & POLLIN) {

			char buffer[1024];
			int bytes_received = recv(clientfd, buffer, 1024, 0);
		
			if (bytes_received > 0) {
				buffer[bytes_received] = '\0';
				std::cout << "Client says: " << buffer << std::endl;
				if (buffer[0] == 'q') {
					std::cout << "Stopping server..." << std::endl;
					break;
				}
			} 
			else if (bytes_received == 0) {
				std::cout << "Client disconnected" << std::endl;
				break;
			} 
			else if (bytes_received == -1) {
				std::cerr << "Failed to receive" << std::endl;
				return 1;
			}
		}

		// Si el fd de entrada estándar tiene datos para leer (POLLIN)
		else if (fds[1].revents & POLLIN) {
			
			char buffer[1024];
			std::cin.getline(buffer, sizeof(buffer));
			strcat(buffer, "\n");
			int bytes_sent = send(clientfd, buffer, strlen(buffer), 0);
			if (bytes_sent == -1) {
				std::cerr << "Failed to send" << std::endl;
				return 1;
			}
		}
	}
	

    std::cout << "Shutting down server..." << std::endl;
    close(serverSocket);
    
    return 0;
}