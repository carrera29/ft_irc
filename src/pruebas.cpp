
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <vector>
#include <map>
#include <sstream>

void commandParser(int clientSocket, const std::string& msg, std::string& nick) {
    std::istringstream iss(msg);
    std::string command;
    iss >> command;

    if (command == "NICK") {
        iss >> nick;
        std::cout << "Client set NICK: " << nick << std::endl;
    }
    else if (command == "CAP") {
        std::string sub;
        iss >> sub;
        if (sub == "LS") {
            std::string capReply =
                ":myserver CAP * LS :multi-prefix sasl\r\n"
                ":myserver CAP * END\r\n";
            send(clientSocket, capReply.c_str(), capReply.size(), 0);
            std::cout << "Responded to CAP LS" << std::endl;
        }
    }
    else if (command == "USER") {
        // Aquí podrías guardar más info si quieres
        std::string username;
        iss >> username;
        std::cout << "Client sent USER: " << username << std::endl;

        if (!nick.empty()) {
            // Handshake mínimo
            std::string welcome =
                ":myserver 001 " + nick + " :Welcome to the IRC test server, " + nick + "\r\n"
                ":myserver 002 " + nick + " :Your host is myserver, running version 0.1\r\n"
                ":myserver 003 " + nick + " :This server was created today\r\n"
                ":myserver 004 " + nick + " myserver 0.1 o o\r\n"
                ":myserver 375 " + nick + " :- myserver Message of the Day -\r\n"
                ":myserver 372 " + nick + " :- Bienvenida a este servidor de pruebas\r\n"
                ":myserver 376 " + nick + " :End of /MOTD command.\r\n";

            send(clientSocket, welcome.c_str(), welcome.size(), 0);
        }
    }
    else {
        std::cout << "Raw IRC Msg: " << msg << std::endl;
    }
}


int main() {

    const char* port = "6667";

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints)); 
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        std::cerr << "Error: getaddrinfo" << std::endl;
        return 1;
    }

    int serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (serverSocket == -1) {
        std::cerr << "Error: socket" << std::endl;
        freeaddrinfo(res);
        return 1;
    }

    int yes = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (bind(serverSocket, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "Error: bind" << std::endl;
        freeaddrinfo(res);
        close(serverSocket);
        return 1;
    }
    freeaddrinfo(res);

    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Error: listen" << std::endl;
        close(serverSocket);
        return 1;
    }
    std::cout << "Server listening on port " << port << "..." << std::endl;

    struct pollfd stdinfd;
    stdinfd.fd = STDIN_FILENO;
    stdinfd.events = POLLIN;

    struct pollfd pfd;
    pfd.fd = serverSocket;
    pfd.events = POLLIN;
    pfd.revents = 0;

    std::vector<pollfd> fds;
    fds.push_back(pfd);
    fds.push_back(stdinfd);

    std::map<int, std::string> clients;

    bool running = true;
    while (running) {
        if (poll(fds.data(), fds.size(), -1) == -1) {
            std::cerr << "Error: poll" << std::endl;
            break;
        }

        for (size_t i = 0; i < fds.size(); ++i) {

            if (fds[i].fd == STDIN_FILENO) {
                char cmd[128];
                if (read(STDIN_FILENO, cmd, sizeof(cmd)) > 0) {
                    if (strncmp(cmd, "quit", 4) == 0) {
                        std::cout << "Shutting down server..." << std::endl;
                        running = false;
                    }
                }
            }
            else if (fds[i].revents & POLLIN) {
                if (fds[i].fd == serverSocket) {
                    int clientSocket = accept(serverSocket, nullptr, nullptr);
                    if (clientSocket != -1) {

                        struct pollfd client;
                        client.fd = clientSocket;
                        client.events = POLLIN;
                        client.revents = 0;
                        fds.push_back(client);
                        std::cout << "New client connected, fd client: " << clientSocket << std::endl;
                    }
                } else {
                    char buffer[1024];
                    int bytes = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes <= 0) {
                        std::cout << "Client disconnected: fd " << fds[i].fd << std::endl;
                        close(fds[i].fd);
                        clients.erase(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        --i;
                    } else {
                        buffer[bytes] = '\0';
                        // std::cout << "Received " << bytes << " bytes: [" << buffer << "]" << std::endl;
                        commandParser(fds[i].fd, std::string(buffer), clients[fds[i].fd]);
                    }
                }
            }
        }
    }
    close(serverSocket);
    return 0;
}
