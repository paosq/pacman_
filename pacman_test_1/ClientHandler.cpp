#include "ClientHandler.h"
#include <iostream>
#include <cstdlib>
#include "Constants.h"

ClientHandler::ClientHandler(SOCKET socket) : clientSocket(socket) {}

void ClientHandler::receiveMessages() {
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "Stan gry: " << buffer << std::endl;
    }

    closesocket(clientSocket);
    std::cout << "Klient rozlaczony." << std::endl;
}
