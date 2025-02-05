#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

class client {

    private:
        
        char            _login[256];
        char            _username[256];
        int             _id;
        int             _clientSocket;
		struct pollfd	_fd;

    public:

        client(int clientSocket, int id) : _clientSocket(clientSocket), _id(id) {
			createPollFd();
            std::cout << "Client id " << id << " connected" << std::endl;
        }

        ~client() {
            close(_clientSocket);
        }

		void	createPollFd() {
			_fd.fd = _clientSocket;
			_fd.events = POLLIN;
			_fd.revents = 0;
		}

        int	getClientSocket() {
            return _clientSocket;
        }

		struct pollfd&	getPollFd() {
			return _fd;
		}

		int	getId() {
			return _id;
		}

		char*	getLogin() {
			return _login;
		}

		char*	getUsername() {
			return _username;
		}

		
        
};
