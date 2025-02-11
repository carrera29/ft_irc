#include <iostream>
#include <sstream>

int main() {

    std::string str = "PASS <password>\r\nNICK <nickname>\r\nUSER <username> 0 * :Real Name\r\n";
    std::stringstream ss(str);

    while (ss) {
        std::string word;
        ss >> word;
        std::cout << word << std::endl;
        if (word == "PASS") {
            ss >> word;
            std::cout << "Password: " << word << std::endl;
        }
    }

    return 0;
}