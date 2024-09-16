#include "Game.h"
#include "Fruit.h"
#include "Entity.h"
#include "Constants.h"
#include "PacMan.h"
#include "Ghost.h"
#include "Controller.h"
#include "ClientHandler.h"

#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <ws2tcpip.h>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")


    Game::Game(bool isSinglePlayer, bool isSimpleMap, Difficulty difficultyLevel, bool ispodglad_gry)
        : pacman1(width / 2, height / 2, 'P'), pacman2(width / 2 + 1, height / 2, 'Q'),
        fruit(), sharedScore(0), singlePlayer(isSinglePlayer), simpleMap(isSimpleMap), difficultyLevel(difficultyLevel), podglad_gry(ispodglad_gry), remainingFruits(0) {
        SetDifficulty(difficultyLevel); // Ustawienie prêdkoœci gry na podstawie poziomu trudnoœci
        ghosts.emplace_back(5, 5, 'G', ghostSpeed); // dodanie ducha jako nowy element wektora // zmienne to pozycja pocz¹tkowa x,y , symbol ducha, szybkoœæ ducha
        ghosts.emplace_back(10, 10, 'G', ghostSpeed);
        ghosts.emplace_back(20, 20, 'G', ghostSpeed);
        //ghosts.emplace_back(5, 5, 'g', ghostSpeed);
    }

    void Game::SetupMap() {
        if (simpleMap) {
            // Prosta mapa
            remainingFruits = 0;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                        map[i][j] = '#';
                        hasFruit[i][j] = false;
                    }
                    else {
                        map[i][j] = ' ';
                        hasFruit[i][j] = true;
                        remainingFruits++; // Liczenie owoców na mapie
                    }
                }
            }
        }
        else {
            // Trudniejsza mapa
            remainingFruits = 0;
            const char predefinedMap[height][width] = {
                "#######################################",
                "#                #####                #",
                "# ###### ####### ##### ####### ###### #",
                "# ###### ####### ##### ####### ###### #",
                "# ###### ####### ##### ####### ###### #",
                "#                                     #",
                "# ######### ######## ########  ###### #",
                "# ######### ###           ###  ###### #",
                "# ######### ###           ###  ###### #",
                "# ######### #################  ###### #",
                "#                                     #",
                "# ###### ####### ##### ####### ###### #",
                "# ###### ####### ##### ####### ###### #",
                "#                                     #",
                "# ###### ####### ##### ####### ###### #",
                "# ###### ####### ##### ####### ###### #",
                "# ###### ####### ##### ####### ###### #",
                "#                 ##                  #",
                "#######################################"
            };

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    map[i][j] = predefinedMap[i][j];
                    hasFruit[i][j] = (map[i][j] == ' '); // Tylko puste miejsca zawieraj¹ owoce
                    if (hasFruit[i][j]) {
                        remainingFruits++; // Liczenie owoców na mapie
                    }
                }
            }
        }
    }

    void Game::Setup() {
        SetupMap();
        pacman1.x = width / 2;
        pacman1.y = height / 2 - 1;
        pacman2.x = width / 2 + 1;
        pacman2.y = height / 2 - 1;
        pacman1.score = 0;
        pacman2.score = 0;
        sharedScore = 0;
        pacman1.dir = STOP;
        pacman2.dir = STOP;

        // Generowanie owoców
        fruit.clear();
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (map[y][x] != '#') {  // jeœli to nie œciana
                    bool isSpecial = (rand() % 100 < 5);  // 5% szans na specjalny owoc
                    Fruit newFruit(x, y, isSpecial);
                    fruit.push_back(newFruit);
                    hasFruit[y][x] = true;
                    remainingFruits++;
                }
                else {
                    hasFruit[y][x] = false;
                }
            }
        }
        std::vector<std::pair<int, int>> corners = {
        {1, 1},                      // Lewy górny róg
        {1, width - 3},              // Prawy górny róg
        {height - 3, 1},             // Lewy dolny róg
        {height - 3, width - 3}      // Prawy dolny róg
        };

        // Losowe umieszczanie duchów w jednym z rogów
        for (auto& ghost : ghosts) {
            int cornerIndex = rand() % corners.size();
            // SprawdŸ, czy miejsce jest puste
            while (map[corners[cornerIndex].first][corners[cornerIndex].second] != ' ') {
                cornerIndex = rand() % corners.size();
            }
            ghost.x = corners[cornerIndex].second;
            ghost.y = corners[cornerIndex].first;
            ghost.dir = static_cast<Direction>(rand() % 4 + 1); // ustawienie losowego kierunku
            ghost.speed = ghostSpeed;
        }
    }

    void Game::Draw() {
        system("cls");

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                // Sprawdzenie, czy na danej pozycji znajduje siê PacMan1
                if (i == pacman1.y && j == pacman1.x) {
                    std::cout << pacman1.symbol;
                }
                // Sprawdzenie, czy na danej pozycji znajduje siê PacMan2 (w trybie dwuosobowym)
                else if (i == pacman2.y && j == pacman2.x && !singlePlayer) {
                    std::cout << pacman2.symbol;
                }
                // Sprawdzenie, czy na danej pozycji znajduje siê duch
                else {
                    bool isGhostHere = false;
                    for (const auto& ghost : ghosts) {
                        if (i == ghost.y && j == ghost.x) {
                            std::cout << ghost.symbol;
                            isGhostHere = true;
                            break;
                        }
                    }
                    // Jeœli nie ma ducha na tej pozycji, sprawdzamy, czy jest owoc

                    if (!isGhostHere) {
                        if (hasFruit[i][j]) {
                            bool isSpecial = false;
                            for (const auto& f : fruit) {
                                if (f.x == j && f.y == i) {
                                    isSpecial = f.isSpecial;
                                    break;
                                }
                            }
                            std::cout << (isSpecial ? '@' : '.');
                        }
                        else {
                            std::cout << map[i][j];
                        }
                    }
                }
            }
            std::cout << std::endl;
        }
        result(true);

    }

    void Game::result(bool check) {
        // Wyœwietlanie wyników
        std::cout << "PacMan1 Score: " << pacman1.score << " zycia: " << pacman1.lives << (check ? " Czas mocy: " + std::to_string(pacman1.GetPowerUpTimeLeft()) : "") << std::endl;
        if (!singlePlayer) {
            std::cout << "PacMan2 Score: " << pacman2.score << " zycia: " << pacman2.lives << (check ? " Czas mocy: " + std::to_string(pacman2.GetPowerUpTimeLeft()) : "") << std::endl;
            sharedScore = pacman1.score + pacman2.score;
        }
        else {
            sharedScore = pacman1.score;
        }

        std::cout << "Shared Score: " << sharedScore << std::endl;
    }

    void Game::Input() {
        controller.GetInput(pacman1, pacman2);
    }

    void Game::Logic() {
        pacman1.Move(map, hasFruit, remainingFruits, fruit);
        if (!singlePlayer) {
            pacman2.Move(map, hasFruit, remainingFruits, fruit);
        }

        for (auto& ghost : ghosts) {
            ghost.Move(map, pacman1); // Duchy œcigaj¹ pierwszego gracza
            if (!singlePlayer) {
                ghost.Move(map, pacman2); // Duchy œcigaj¹ drugiego gracza
            }
            ghost.setisg(pacman1.powerUpActive || (pacman2.powerUpActive && !singlePlayer));
        }

        // Sprawdzenie kolizji z duchem
        CheckCollision();

        // Sprawdzenie wygranej
        if (remainingFruits <= 0) {
            system("cls");
            result(false);
            exit(0);
        }
    }

    void Game::GameOver() {

        system("cls");
        result(false);

        exit(0);
    }

    void Game::CheckCollision() {
        for (auto& ghost : ghosts) {
            // Sprawdzanie kolizji PacMana1 z duchem
            if (pacman1.x == ghost.x && pacman1.y == ghost.y) {
                if (pacman1.powerUpActive) {
                    // PacMan1 ma aktywn¹ moc - zjada ducha
                    ghost.x = 1 + rand() % (width - 2);
                    ghost.y = 1 + rand() % (height - 2);
                    pacman1.score += 100; // Nagroda za zjedzenie ducha
                }
                else {
                    // PacMan1 traci ¿ycie
                    pacman1.lives--;
                    if (pacman1.lives <= 0) {
                        GameOver();
                    }
                    else {
                        // Reset pozycji PacMana1
                        pacman1.x = width / 2;
                        pacman1.y = height / 2;
                    }
                }
            }

            // Sprawdzanie kolizji PacMana2 z duchem 
            if (!singlePlayer && pacman2.x == ghost.x && pacman2.y == ghost.y) {
                if (pacman2.powerUpActive) {
                    // PacMan2 ma aktywn¹ moc - zjada ducha
                    ghost.x = 1 + rand() % (width - 2);
                    ghost.y = 1 + rand() % (height - 2);
                    pacman2.score += 100; // Nagroda za zjedzenie ducha
                }
                else {
                    // PacMan2 traci ¿ycie
                    pacman2.lives--;
                    if (pacman2.lives <= 0) {
                        GameOver();
                    }
                    else {
                        // Reset pozycji PacMana2
                        pacman2.x = width / 2 + 1;
                        pacman2.y = height / 2;
                    }
                }
            }
        }
    }

    void Game::Run() {

        const std::chrono::milliseconds frameDuration(gameSpeed);   // Definiowanie czasu trwania klatki
        auto nextFrameTime = std::chrono::steady_clock::now() + frameDuration;      // Czas, kiedy powinna rozpocz¹æ siê nastêpna klatka

        Setup();        //³adowanie wszystkich ustawieñ

        while (true) {
            auto frameStart = std::chrono::steady_clock::now();
            Draw();
            Input();
            Logic();

            //if (podglad_gry == 0) sendGameState();

            auto frameEnd = std::chrono::steady_clock::now();   // Czas zakoñczenia klatki
            auto frameTime = frameEnd - frameStart;     // Obliczanie czasu trwania klatki
            auto sleepDuration = frameDuration - frameTime;     // Obliczanie czasu, jaki nale¿y odczekaæ do nastêpnej klatki

            if (sleepDuration > std::chrono::milliseconds(0)) std::this_thread::sleep_for(sleepDuration);   // Jeœli czas snu jest dodatni, usypiamy
            nextFrameTime += frameDuration;// Obliczanie czasu do nastêpnej klatki
        }
    }

    void Game::ShowMenu(bool& isSinglePlayer, bool& isSimpleMap, bool& ispodglad_gry) {
        char choice;

        // Tryb gry
        std::cout << "Wybierz tryb gry: " << std::endl;
        std::cout << "1. Gra jednoosobowa" << std::endl;
        std::cout << "2. Gra dwuosobowa" << std::endl;
        choice = _getch();

        if (choice == '1') {
            isSinglePlayer = true;
        }
        else if (choice == '2') {
            isSinglePlayer = false;
        }
        else {
            std::cout << "Niepoprawny wybor, domyslnie ustawiono gre jednoosobowa." << std::endl;
            isSinglePlayer = true;
        }

        // Typ mapy
        std::cout << "Wybierz typ mapy: " << std::endl;
        std::cout << "1. Prosta mapa" << std::endl;
        std::cout << "2. Trudna mapa" << std::endl;
        choice = _getch();

        if (choice == '1') {
            isSimpleMap = true;
        }
        else if (choice == '2') {
            isSimpleMap = false;
        }
        else {
            std::cout << "Niepoprawny wybor, domyslnie ustawiono prosta mape." << std::endl;
            isSimpleMap = true;
        }

        // poziom trudnoœci
        std::cout << "Wybierz poziom trudnosci: " << std::endl;
        std::cout << "1. Latwy" << std::endl;
        std::cout << "2. Sredni" << std::endl;
        std::cout << "3. Trudny" << std::endl;
        choice = _getch();

        switch (choice) {
        case '1': difficultyLevel = EASY; break;
        case '2': difficultyLevel = MEDIUM; break;
        case '3': difficultyLevel = HARD; break;
        default:
            std::cout << "Niepoprawny wybor, domyslnie ustawiono poziom latwy." << std::endl;
            difficultyLevel = EASY;
            break;
        }

        // pogl¹d po sokecie
        std::cout << "Chcesz podglad gry w 2 oknie: " << std::endl;
        std::cout << "1. NIE" << std::endl;
        std::cout << "2. TAK" << std::endl;
        choice = _getch();

        if (choice == '1') {
            ispodglad_gry = true;
        }
        else if (choice == '2') {
            ispodglad_gry = false;
        }
        else {
            std::cout << "Niepoprawny wybor, domyslnie brak podgl¹du." << std::endl;
            ispodglad_gry = true;
        }

        // informacje o sterowaniu
        system("cls");
        std::cout << "Instrukcja " << std::endl << std::endl;
        std::cout << "Gre mozna zakonczyc w dowolnym momecie naciskajac: x " << std::endl;
        std::cout << "Przeciwnicy to: G i g" << std::endl;
        std::cout << "Niueprzekraczalna bariera to: #" << std::endl;
        std::cout << "Punkty ktore musisz zebrac to: ." << std::endl;
        std::cout << "Owoce specjalne pozwalajace na zjadanie przeciwnikow to: @" << std::endl;
        std::cout << "Owoce specjalne pozwalajace na zjadanie tylko w ograniczanym czasie ktorego czas jest sygnalizowany na dole ekranu zaraz za punktami" << std::endl;
        std::cout << std::endl; std::cout << std::endl;

        std::cout << "Postac ma symbol: P" << std::endl;
        std::cout << "Sterowanie to:" << std::endl;
        std::cout << "Gora: w" << std::endl;
        std::cout << "Dol: s" << std::endl;
        std::cout << "Lewo: a" << std::endl;
        std::cout << "Prawo: d" << std::endl;
        std::cout << std::endl;

        if (isSinglePlayer == false) {
            std::cout << "Postac ma symbol: Q" << std::endl;
            std::cout << "Sterowanie to:" << std::endl;
            std::cout << "Gora: i" << std::endl;
            std::cout << "Dol: k" << std::endl;
            std::cout << "Lewo: j" << std::endl;
            std::cout << "Prawo: l" << std::endl;
            std::cout << std::endl;
        }
        std::cout << " Nacisnij dowolny klawisz aby przejsc dalej" << std::endl;
        choice = _getch();

    }

    void Game::SetDifficulty(Difficulty level) {
        difficultyLevel = level;
        switch (difficultyLevel) {
        case EASY:
            gameSpeed = 300; // Szybkoœæ dla poziomu ³atwego
            ghostSpeed = 1;  // Prêdkoœæ duchów na poziomie ³atwym
            break;
        case MEDIUM:
            gameSpeed = 200; // Szybkoœæ dla poziomu œredniego
            ghostSpeed = 2;  // Prêdkoœæ duchów na poziomie œrednim
            break;
        case HARD:
            gameSpeed = 100; // Szybkoœæ dla poziomu trudnego
            ghostSpeed = 3;  // Prêdkoœæ duchów na poziomie trudnym
            break;
        default:
            gameSpeed = 300; // Domyœlna prêdkoœæ
            ghostSpeed = 1;  // Domyœlna prêdkoœæ duchów
            break;
        }
    }

    void Game::sendGameState(SOCKET clientSocket) {
        // Serializacja stanu gry jako string
        std::string gameState;

        // Serializacja mapy gry
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                gameState += map[i][j];
            }
            gameState += '\n'; // nowa linia dla ka¿dej linii mapy
        }

        // Dodanie pozycji PacMana i duchów
        gameState += "PacMan1: " + std::to_string(pacman1.x) + ", " + std::to_string(pacman1.y) + '\n';
        gameState += "PacMan2: " + std::to_string(pacman2.x) + ", " + std::to_string(pacman2.y) + '\n';

        for (const auto& ghost : ghosts) {
            gameState += "Ghost: " + std::to_string(ghost.x) + ", " + std::to_string(ghost.y) + '\n';
        }

        // Wys³anie stanu gry do klienta
        int totalBytesSent = 0;
        int bytesRemaining = gameState.size();

        while (totalBytesSent < gameState.size()) {
            int bytesSent = send(clientSocket, gameState.c_str() + totalBytesSent, bytesRemaining, 0);
            //std::cout << "Blad wysylania stanu gry! Kod bledu: " <<
            if (bytesSent == SOCKET_ERROR) {
                std::cout << "Blad wysylania stanu gry! Kod bledu: " << WSAGetLastError() << std::endl;
                return;
            }
            totalBytesSent += bytesSent;
            bytesRemaining -= bytesSent;
        }
    }

    bool Server::start() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cout << "Nie mozna zainicjowac Winsock!" << std::endl;
            return false;
        }

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            std::cout << "Nie mozna utworzyc gniazda!" << std::endl;
            WSACleanup();
            return false;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cout << "Blad bindowania gniazda!" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return false;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cout << "Blad nasluchiwania na gniezdzie!" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return false;
        }

        std::cout << "Serwer uruchomiony i nasluchuje na porcie " << PORT << std::endl;
        return true;
    }

    void Server::acceptClient(Game& game) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Blad akceptacji polaczenia!" << std::endl;
            return;
        }

        std::cout << "Nowy klient polaczony." << std::endl;
        ClientHandler handler(clientSocket);
        /*
        std::thread receiverThread(&ClientHandler::receiveMessages, &handler);
        handler.sendMessages();
        */
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // opóŸnienia (np. 1 sekunda) miêdzy wysy³aniem stanu gry
            game.sendGameState(clientSocket);
        }

        //if (receiverThread.joinable()) receiverThread.join();
        // Po zakoñczeniu zamykam w¹tek
        closesocket(clientSocket);
        std::cout << "Klient roz³¹czony." << std::endl;
    }

    Server::~Server() {
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
            WSACleanup();
        }
    }

#include <thread>
#include <mutex>

    std::mutex gameStateMutex; // Mutex do synchronizacji dostêpu do stanu gry

    void sendGameStateThread(Game* game, SOCKET clientSocket) {
        std::lock_guard<std::mutex> lock(gameStateMutex);
        game->sendGameState(clientSocket);
    }