#include <iostream>
#include <conio.h> // getch() for capturing key presses
#include <windows.h> // Sleep() for delaying execution
#include <cstdlib> // For rand()
#include <vector> // dodanie duchów

using namespace std;

const int width = 40;
const int height = 20;
const int game_speed = 300;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum Difficulty { EASY, MEDIUM, HARD };

class Entity {
public:
    int x, y;
    Entity(int startX, int startY) : x(startX), y(startY) {}
};

class PacMan : public Entity {
public:
    Direction dir;
    char symbol;
    int score;
    int lives; //liczba żyć

    PacMan(int startX, int startY, char sym) : Entity(startX, startY), dir(STOP), symbol(sym), score(0), lives(3) {}

    void Move(char map[height][width], bool hasFruit[height][width], int& remainingFruits) {
        int nextX = x;
        int nextY = y;

        switch (dir) {
        case LEFT:  nextX--; break;
        case RIGHT: nextX++; break;
        case UP:    nextY--; break;
        case DOWN:  nextY++; break;
        default:    break;
        }

        // sprawdzenie zderzenia z mapą
        if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height && map[nextY][nextX] != '#') {
            x = nextX;
            y = nextY;

            //sprawdzenie zjedzenia owocu
            if (hasFruit[y][x]) {
                score += 10;
                hasFruit[y][x] = false;
                remainingFruits--; // Zmniejszenie liczby pozostałych owoców
            }
        }
        else {
            // zatrzymanie jeśli zderzenie
            dir = STOP;
        }
    }
};

class Ghost : public Entity {
public:
    Direction dir;
    char symbol;
    int speed; // szybkość duchów w zależnośći od poziomu

    Ghost(int startX, int startY, char sym, int spd)
        : Entity(startX, startY), dir(STOP), symbol(sym), speed(spd) {}

    void Move(char map[height][width], const PacMan& pacman) { // metoda losowa z priorytetem
        int nextX = x;
        int nextY = y;

        // wyliczanie dystansu od gracza
        int deltaX = pacman.x - x;
        int deltaY = pacman.y - y;

        Direction possibleDirections[4] = { STOP, STOP, STOP, STOP };
        int numDirections = 0;

        if (deltaX < 0 && map[y][x - 1] != '#') {
            possibleDirections[numDirections++] = LEFT;
        }
        if (deltaX > 0 && map[y][x + 1] != '#') {
            possibleDirections[numDirections++] = RIGHT;
        }
        if (deltaY < 0 && map[y - 1][x] != '#') {
            possibleDirections[numDirections++] = UP;
        }
        if (deltaY > 0 && map[y + 1][x] != '#') {
            possibleDirections[numDirections++] = DOWN;
        }

        if (numDirections == 0) {
            Direction randomDirections[4] = { LEFT, RIGHT, UP, DOWN };
            dir = randomDirections[rand() % 4];
        }
        else {
            dir = possibleDirections[rand() % numDirections];
        }

        // Aktualizacja pozycji na podstawie wybranego kierunku
        switch (dir) {
        case LEFT:  nextX -= speed; break;
        case RIGHT: nextX += speed; break;
        case UP:    nextY -= speed; break;
        case DOWN:  nextY += speed; break;
        default:    break;
        }

        // Sprawdzenie kolizji z mapą
        if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height && map[nextY][nextX] != '#') {
            x = nextX;
            y = nextY;
        }
    }
};

class Fruit : public Entity {
public:
    Fruit(int startX, int startY) : Entity(startX, startY) {}
    void Respawn(char map[height][width]) {
        do {
            x = 1 + rand() % (width - 2);
            y = 1 + rand() % (height - 2);
        } while (map[y][x] != ' ');
    }
};


class Controller {
public:
    void GetInput(PacMan& pacman1, PacMan& pacman2) {
        if (_kbhit()) {
            switch (_getch()) {
            case 'a': pacman1.dir = LEFT; break;
            case 'd': pacman1.dir = RIGHT; break;
            case 'w': pacman1.dir = UP; break;
            case 's': pacman1.dir = DOWN; break;
            case 'j': pacman2.dir = LEFT; break;
            case 'l': pacman2.dir = RIGHT; break;
            case 'i': pacman2.dir = UP; break;
            case 'k': pacman2.dir = DOWN; break;
            case 'x': exit(0); // Exit the game
            }
        }
    }
};

class Game {
private:
    PacMan pacman1;
    PacMan pacman2;
    Fruit fruit;
    Controller controller;
    vector<Ghost> ghosts; // Vector to manage ghosts
    int sharedScore;
    char map[height][width]; // Map with walls
    bool hasFruit[height][width];// Mapa owoców
    bool singlePlayer;  // Flaga dla trybu gry
    bool simpleMap;     // Flaga dla wyboru mapy
    Difficulty difficultyLevel; // Poziom trudności
    int gameSpeed;
    int ghostSpeed; // prędkość ducha
    int remainingFruits; // Liczba pozostałych owoców

public:

    Game(bool isSinglePlayer, bool isSimpleMap, Difficulty difficultyLevel)
        : pacman1(width / 2, height / 2, 'P'), pacman2(width / 2 + 1, height / 2, 'Q'),
        fruit(0, 0), sharedScore(0), singlePlayer(isSinglePlayer), simpleMap(isSimpleMap), difficultyLevel(difficultyLevel), remainingFruits(0) {
        SetDifficulty(difficultyLevel); // Ustawienie prędkości gry na podstawie poziomu trudności
        SetupMap();// Initializacja mapy ze ścianami
        ghosts.emplace_back(5, 5, 'G', ghostSpeed); // dodanie ducha jako nowy element wektora // zmienne to pozycja początkowa x,y , symbol ducha, szybkość ducha
        ghosts.emplace_back(10, 10, 'g', ghostSpeed);
        //ghosts.emplace_back(5, 5, 'g', ghostSpeed);
    }

    void SetupMap() {
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
                    hasFruit[i][j] = (map[i][j] == ' '); // Tylko puste miejsca zawierają owoce
                    if (hasFruit[i][j]) {
                        remainingFruits++; // Liczenie owoców na mapie
                    }
                }
            }
        }
    }
    
    void Setup() {
        pacman1.x = width / 2;
        pacman1.y = height / 2-1;
        pacman2.x = width / 2 + 1;
        pacman2.y = height / 2-1;
        fruit.Respawn(map);
        pacman1.score = 0;
        pacman2.score = 0;
        sharedScore = 0;
        pacman1.dir = STOP;
        pacman2.dir = STOP;

        vector<pair<int, int>> corners = {
        {1, 1},                      // Lewy górny róg
        {1, width- 3},              // Prawy górny róg
        {height - 3, 1},             // Lewy dolny róg
        {height - 3, width - 3}      // Prawy dolny róg
        };

        // Losowe umieszczanie duchów w jednym z rogów
        for (auto& ghost : ghosts) {
            int cornerIndex = rand() % corners.size();
            // Sprawdź, czy miejsce jest puste
            while (map[corners[cornerIndex].first][corners[cornerIndex].second] != ' ') {
                cornerIndex = rand() % corners.size();
            }
            ghost.x = corners[cornerIndex].second;
            ghost.y = corners[cornerIndex].first;
            ghost.dir = static_cast<Direction>(rand() % 4 + 1); // ustawienie losowego kierunku
            ghost.speed = ghostSpeed;
        }
    }

    void Draw() {
        system("cls");

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                // Sprawdzenie, czy na danej pozycji znajduje się PacMan1
                if (i == pacman1.y && j == pacman1.x) {
                    cout << pacman1.symbol;
                }
                // Sprawdzenie, czy na danej pozycji znajduje się PacMan2 (w trybie dwuosobowym)
                else if (i == pacman2.y && j == pacman2.x && !singlePlayer) {
                    cout << pacman2.symbol;
                }
                // Sprawdzenie, czy na danej pozycji znajduje się duch
                else {
                    bool isGhostHere = false;
                    for (const auto& ghost : ghosts) {
                        if (i == ghost.y && j == ghost.x) {
                            cout << ghost.symbol;
                            isGhostHere = true;
                            break;
                        }
                    }
                    // Jeśli nie ma ducha na tej pozycji, sprawdzamy, czy jest owoc
                    if (!isGhostHere) {
                        if (hasFruit[i][j]) {
                            cout << '.';
                        }
                        else {
                            cout << map[i][j];
                        }
                    }
                }
            }
            cout << endl;
        }

        // Wyświetlanie wyników
        cout << "PacMan1 Score: " << pacman1.score << " zycia: " << pacman1.lives << endl;
        if (!singlePlayer) {
            cout << "PacMan2 Score: " << pacman2.score << " zycia: " << pacman2.lives << endl;
            sharedScore = pacman1.score + pacman2.score;
        }
        else {
            sharedScore = pacman1.score;
        }

        cout << "Shared Score: " << sharedScore << endl;
    }

    void Input() {
        controller.GetInput(pacman1, pacman2);
    }

    void Logic() {
        pacman1.Move(map,hasFruit, remainingFruits);
        if (!singlePlayer) {
            pacman2.Move(map, hasFruit, remainingFruits);
        }

        for (auto& ghost : ghosts) {
            ghost.Move(map, pacman1); // Duchy ścigają pierwszego gracza
            if (!singlePlayer) {
                ghost.Move(map, pacman2); // Duchy ścigają drugiego gracza
            }
        }
        // Sprawdzenie kolizji z duchem
        CheckCollision();

        // Sprawdzenie wygranej
        if (remainingFruits <= 0) {
            system("cls");
            cout << "Congratulations! You've won the game!" << endl;
            cout << "PacMan1 Score: " << pacman1.score << endl;
            if (!singlePlayer) {
                cout << "PacMan2 Score: " << pacman2.score << endl; 
                sharedScore = pacman1.score + pacman2.score;
            }
            else {
                sharedScore = pacman1.score;
            }
            cout << "Shared Score: " << sharedScore << endl;
            Sleep(1000);
            exit(0);
        }
    }

    void CheckCollision() {
        // Kolizja PacMana z duchami
        for (const Ghost& ghost : ghosts) {
            if ((pacman1.x == ghost.x && pacman1.y == ghost.y) || (pacman2.x == ghost.x && pacman2.y == ghost.y)) {
                if (pacman1.x == ghost.x && pacman1.y == ghost.y) {
                    pacman1.lives--;
                    if (pacman1.lives <= 0) {
                        system("cls");
                        cout << "PacMan1 Score: " << pacman1.score << endl;
                        if (!singlePlayer) {
                            cout << "PacMan2 Score: " << pacman2.score << endl;
                            sharedScore = pacman1.score + pacman2.score;
                        }
                        else {
                            sharedScore = pacman1.score;
                        }
                        cout << "Shared Score: " << sharedScore << endl;
                        Sleep(1000);
                        exit(0);
                    }
                    pacman1.x = width / 2;
                    pacman1.y = height / 2;
                }
                if (pacman2.x == ghost.x && pacman2.y == ghost.y) {
                    pacman2.lives--;
                    if (pacman2.lives <= 0) {
                        system("cls");
                        cout << "Gracz 2 - Przegrał! Game Over!" << endl;
                        cout << "PacMan1 Score: " << pacman1.score << endl;
                        if (!singlePlayer) {
                            cout << "PacMan2 Score: " << pacman2.score << endl;
                            sharedScore = pacman1.score + pacman2.score;
                        }
                        else {
                            sharedScore = pacman1.score;
                        }
                        cout << "Shared Score: " << sharedScore << endl;
                        Sleep(1000);
                        exit(0);
                    }
                    pacman2.x = width / 2 + 1;
                    pacman2.y = height / 2;
                }
            }
        }
    }


    void Run() {
        Setup();
        while (true) {
            Draw();
            Input();
            Logic();
            Sleep(game_speed); // szybkość gry
        }
    }

    void ShowMenu(bool& isSinglePlayer, bool& isSimpleMap) {
        char choice;

        // Tryb gry
        cout << "Wybierz tryb gry: " << endl;
        cout << "1. Gra jednoosobowa" << endl;
        cout << "2. Gra dwuosobowa" << endl;
        Sleep(300);
        choice = _getch();

        if (choice == '1') {
            isSinglePlayer = true;
        }
        else if (choice == '2') {
            isSinglePlayer = false;
        }
        else {
            cout << "Niepoprawny wybor, domyslnie ustawiono gre jednoosobowa." << endl;
            isSinglePlayer = true;
        }

        // Typ mapy
        cout << "Wybierz typ mapy: " << endl;
        cout << "1. Prosta mapa" << endl;
        cout << "2. Trudna mapa" << endl;
        Sleep(300);
        choice = _getch();

        if (choice == '1') {
            isSimpleMap = true;
                Sleep(300);
        }
        else if (choice == '2') {
            isSimpleMap = false;
        }
        else {
            cout << "Niepoprawny wybor, domyslnie ustawiono prosta mape." << endl;
            isSimpleMap = true;
            Sleep(500);
        }

        // poziom trudności
        cout << "Wybierz poziom trudności: " << endl;
        cout << "1. Latwy" << endl;
        cout << "2. Sredni" << endl;
        cout << "3. Trudny" << endl;
        choice = _getch();
        Sleep(300);

        switch (choice) {
        case '1': difficultyLevel = EASY; break;
        case '2': difficultyLevel = MEDIUM; break;
        case '3': difficultyLevel = HARD; break;
        default:
            cout << "Niepoprawny wybor, domyslnie ustawiono poziom latwy." << endl;
            difficultyLevel = EASY;
            break;
        }
    }

    void SetDifficulty(Difficulty level) {
        difficultyLevel = level;
        switch (difficultyLevel) {
        case EASY:
            gameSpeed = 300; // Szybkość dla poziomu łatwego
            ghostSpeed = 1;  // Prędkość duchów na poziomie łatwym
            break;
        case MEDIUM:
            gameSpeed = 200; // Szybkość dla poziomu średniego
            ghostSpeed = 2;  // Prędkość duchów na poziomie średnim
            break;
        case HARD:
            gameSpeed = 100; // Szybkość dla poziomu trudnego
            ghostSpeed = 3;  // Prędkość duchów na poziomie trudnym
            break;
        default:
            gameSpeed = 300; // Domyślna prędkość
            ghostSpeed = 1;  // Domyślna prędkość duchów
            break;
        }
    }
};

int main() {
    srand(time(0));

    bool isSinglePlayer = true;  // Inicjalizacja zmiennej domyślną wartością
    bool isSimpleMap = true;     // Inicjalizacja zmiennej domyślną wartością
    Difficulty difficultyLevel = EASY;

    Game game(isSinglePlayer, isSimpleMap, difficultyLevel); // Wywołanie ShowMenu przed utworzeniem obiektu Game
    game.ShowMenu(isSinglePlayer, isSimpleMap); // Teraz ShowMenu ustawi te wartości

    game = Game(isSinglePlayer, isSimpleMap, difficultyLevel);

    game.Run();
    return 0;
}

