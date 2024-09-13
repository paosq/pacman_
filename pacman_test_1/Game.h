// Game.h
#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include "PacMan.h"
#include "Fruit.h"
#include "Ghost.h"
#include "Controller.h"
#include "Constants.h"
#include "ClientHandler.h"
#include <mutex>


class Game {
private:
    SOCKET serverSocket;
    PacMan pacman1;
    PacMan pacman2;
    std::vector<Fruit> fruit;
    Controller controller;
    std::vector<Ghost> ghosts;
    int sharedScore;
    char map[height][width];
    bool hasFruit[height][width];
    bool singlePlayer;
    bool simpleMap;
    bool podglad_gry;
    Difficulty difficultyLevel;
    int gameSpeed;
    int ghostSpeed;
    int remainingFruits;

public:
    Game(bool isSinglePlayer, bool isSimpleMap, Difficulty difficultyLevel, bool ispodglad_gry);

    void SetupMap();
    void Setup();
    void Draw();
    void result(bool check);
    void Input();
    void Logic();
    void GameOver();
    void CheckCollision();
    void Run();
    void ShowMenu(bool& isSinglePlayer, bool& isSimpleMap, bool& ispodglad_gry);
    void SetDifficulty(Difficulty level);
    void sendGameState(SOCKET clientSocket);
};

class Server {
private:
    SOCKET serverSocket;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool clientConnected_ = false;

public:
    Server() : serverSocket(INVALID_SOCKET) {}
    bool start();
    void acceptClient(Game& game);
    ~Server();
};


#endif // GAME_H
