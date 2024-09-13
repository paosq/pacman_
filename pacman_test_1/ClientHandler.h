#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <winsock2.h>
#include <string>
#include <cstdlib>

class ClientHandler {
private:
    SOCKET clientSocket;

public:
    ClientHandler(SOCKET socket);
    void receiveMessages();
};

#endif // CLIENT_HANDLER_H
