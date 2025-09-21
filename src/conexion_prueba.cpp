// servidor_simple.cpp  (c++98)
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <string>

void send_all(int fd, const std::string &s) {
    ssize_t r = send(fd, s.c_str(), s.size(), 0);
    if (r == -1) std::cerr << "Warning: send() failed\n";
}

void handle_line(int clientSocket, const std::string &line, std::string &nick) {
    std::istringstream iss(line);
    std::string command;
    iss >> command;

    if (command == "CAP") {
        std::cout << "Raw IRC Msg: " << line << "\n";
        std::string sub; iss >> sub;
        if (sub == "LS") {
            std::string capReply =
                ":myserver CAP * LS :multi-prefix sasl\r\n"
                ":myserver CAP * END\r\n";
            send_all(clientSocket, capReply);
            std::cout << "Responded to CAP LS\n";
        }
    }
    else if (command == "NICK") {
        std::cout << "Raw IRC Msg: " << line << "\n";
        std::string newnick; iss >> newnick;
        nick = newnick;
        std::cout << "Client set NICK: " << nick << "\n";
    }
    else if (command == "USER") {
        std::cout << "Raw IRC Msg: " << line << "\n";
        std::string username; iss >> username;
        std::cout << "Client sent USER: " << username << "\n";

        if (!nick.empty()) {
            std::string welcome;
            welcome += ":myserver 001 " + nick + " :Welcome to the IRC test server, " + nick + "\r\n";
            welcome += ":myserver 002 " + nick + " :Your host is myserver, running version 0.1\r\n";
            welcome += ":myserver 003 " + nick + " :This server was created today\r\n";
            welcome += ":myserver 004 " + nick + " myserver 0.1 o o\r\n";
            welcome += ":myserver 375 " + nick + " :- myserver Message of the Day -\r\n";
            welcome += ":myserver 372 " + nick + " :- Bienvenida a este servidor de pruebas\r\n";
            welcome += ":myserver 376 " + nick + " :End of /MOTD command.\r\n";
            send_all(clientSocket, welcome);
        }
    }
    else {
        std::cout << "Raw IRC Msg: " << line << "\n";
    }
}

int main() {
    const char *port = "6667";
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        std::cerr << "Error: getaddrinfo\n";
        return 1;
    }

    int serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (serverSocket == -1) {
        std::cerr << "Error: socket\n";
        freeaddrinfo(res);
        return 1;
    }

    int yes = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    if (bind(serverSocket, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "Error: bind\n";
        freeaddrinfo(res);
        close(serverSocket);
        return 1;
    }
    freeaddrinfo(res);

    if (listen(serverSocket, 1) == -1) {
        std::cerr << "Error: listen\n";
        close(serverSocket);
        return 1;
    }
    std::cout << "Server listening on port " << port << "...\n";

    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == -1) {
        std::cerr << "Error: accept\n";
        close(serverSocket);
        return 1;
    }
    std::cout << "Client connected, fd: " << clientSocket << "\n";

    std::string accbuf;
    char buf[1024];
    std::string nick;

    while (1) {
        ssize_t bytes = recv(clientSocket, buf, sizeof(buf) - 1, 0);
        if (bytes <= 0) {
            std::cout << "Client disconnected or recv error\n";
            break;
        }
        buf[bytes] = '\0';
        accbuf += buf;

        // procesar líneas completas (primero \r\n, si no existe busca \n)
        while (1) {
            size_t pos = accbuf.find("\r\n");
            size_t pos2 = std::string::npos;
            if (pos == std::string::npos) pos2 = accbuf.find('\n');
            if (pos == std::string::npos && pos2 == std::string::npos) break;
            size_t epos = (pos != std::string::npos) ? pos : pos2;
            std::string line = accbuf.substr(0, epos);
            if (!line.empty() && line[line.size()-1] == '\r') line.erase(line.size()-1);
            accbuf.erase(0, epos + ((pos != std::string::npos) ? 2 : 1));
            std::cout << "Received line: [" << line << "]\n";
            handle_line(clientSocket, line, nick);
        }
    }

    close(clientSocket);
    close(serverSocket);
    return 0;
}
