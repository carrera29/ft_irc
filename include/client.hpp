#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cstring>

class client {

    private:
        
        char            _nickname[256];
        char            _username[256];
		bool			_operator;
        int             _id;
        int             _clientSocket;
		struct pollfd	_fd;

    public:

        client(int clientSocket, int id) : _clientSocket(clientSocket), _id(id), _operator(false) {
            std::cout << "Client id " << id << " connected" << std::endl;
        }

        ~client() {
            close(_clientSocket);
			std::cout << "Client id " << _id << " disconnected" << std::endl;
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

		std::string	getNickname() {
			return _nickname;
		}

		std::string	getUsername() {
			return _username;
		}

		
        
};
