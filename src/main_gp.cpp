
#include "server.hpp"
#include "channel.hpp"
#include "client.hpp"

int main() {
    server myServer("pass123", "6667");

    client* user1 = myServer.createClient("Alice", "alice123", 1);
    // client* user2 = myServer.createClient("Bob", "bob321", 2);
    // client* user3 = myServer.createClient("Carol", "carolX", 3);

    myServer.handleJoin("General", "General", user1);

    // myServer.handleJoin("General", "Canal de charla", user2);

    // myServer.handleJoin("General", "", user3);

    // myServer.handleTopic("#General", "Bienvenidos a #General");

    // myServer.handleInvite("#General", user3);

    myServer.handleMode("#General", "i", true);
    myServer.handleMode("#General", "t", false);
    myServer.handleMode("#General", "k", true);
    myServer.handleMode("#General", "o", false);
    myServer.handleMode("#General", "l", 10);

    // myServer.handleKick("#General", user2);

    // myServer.handleInvite("#General", user2);

    // myServer.channels[0]->banUser(user3);
    // myServer.handleInvite("#General", user3);

    // myServer.handleTopic("#General", "");

    // myServer.handleInvite("#Desconocido", user1);

    myServer.removeClient(1);
    myServer.removeClient(2);
    myServer.removeClient(3);

    
    return 0;
}
