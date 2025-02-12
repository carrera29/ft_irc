#include    <iostream>
#include    <sys/socket.h>
#include    <netdb.h>
#include    <string.h>
#include    <unistd.h>
#include    <cstring>
#include    <poll.h>
#include    <vector>
#include    <map>

class Server
{
    private:

        std::string                 _password;
        int                         _port;
        int                         _serverSocket;
        struct  addrinfo            _hints;

    public:

        std::vector<struct pollfd>				fds;
        std::map<int, class client *>			clients;
        std::map<int, class client *>::iterator	it;

        Server(std::string password, int port) : _password(password), _port(port) {}

        ~Server() {
            std::cout << "Server disconnected" << std::endl;
        }

        void setAddressInfo() {
            memset(&_hints, 0, sizeof(_hints));
            _hints.ai_family = AF_UNSPEC;
            _hints.ai_socktype = SOCK_STREAM;
            _hints.ai_flags = AI_PASSIVE;
        } 

        std::string getPassword() {
            return _password;
        }

        struct  addrinfo    *getHints() {
            return &_hints;
        }

        int    getSocket() {
            return _serverSocket;
        }

        void   setSocket(int socket) {
            _serverSocket = socket;
        }
};