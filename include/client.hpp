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

        client(int clientSocket) : _operator(false), _socket(clientSocket) {
            memset(_nickname, 0, sizeof(_nickname));
            memset(_username, 0, sizeof(_username));
        }

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
