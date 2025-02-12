#include <iostream>
#include <sstream>
#include <vector>
#include <string>

int main() {

    std::string str = "@aaa=bbb;ccc;example.com/ddd=eee :nick!ident@host.com PRIVMSG me :Hello\r\n";
    // std::string str = ":nick!ident@host.com PRIVMSG me :Hello";
    std::stringstream ss(str);
    int     nComm = 0;

    while (ss) {
        std::string command;
        ss >> command;

        if (nComm == 0 && command[0] == '@') {
            std::string tag = command.substr(1, command.length());
            std::vector<std::string> AllTags;
            
            int i = 0;
            while ((i = command.find(";")) != std::string::npos) {
                AllTags.push_back(command.substr(0, i));
                command.erase(0, i + 1);
            }
            AllTags.push_back(command);

            for (size_t i = 0; i < AllTags.size(); i++)
                std::cout << "Tag " << i + 1 << ": " << AllTags[i] << std::endl;
            nComm++;
        }
        else if (nComm < 2 && command[0] == ':') {
            // https://modern.ircdocs.horse/#client-messages
            std::string source = command.substr(1, command.length());
            std::cout << "Source: " << source << std::endl;
            nComm += 2;
        }
        else if (nComm > 1) {
            if (command[0] == ':') {
                std::string msg = command.substr(1, command.length());
                std::cout << "Msg: " << msg << std::endl;
            }
        }
        else {
            std::cout << "Error: format msg" << std::endl;
            break;
        }
    }
    return 0;
}