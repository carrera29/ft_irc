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
        int             _socket;


    public:

        client(int clientSocket) : _socket(clientSocket), _operator(false) {}

        ~client() {
            close(_socket);
        }


        int	getClientSocket() {
            return _socket;
        }

		std::string	getNickname() {
			return _nickname;
		}

		std::string	getUsername() {
			return _username;
		}
};
