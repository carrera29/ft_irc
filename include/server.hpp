#include    <iostream>
#include    <sys/socket.h>
#include    <netdb.h>
#include    <string.h>
#include    <unistd.h>

class Server
{
    public:
        Server();
        ~Server();

    private:
        int     serverSocket;
        struct  addrinfo hints;
        struct  addrinfo *all, *i;
        char    buffer[256];
};