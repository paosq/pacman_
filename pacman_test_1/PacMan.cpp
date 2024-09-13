#include "PacMan.h"
#include <cstdlib> // dla funkcji rand()
#include "Constants.h"


PacMan::PacMan(int startX, int startY, char sym)
    : Entity(startX, startY), dir(STOP), symbol(sym), score(0), lives(3), powerUpActive(false), powerUpTimer(0) {}

void PacMan::Move(char map[height][width], bool hasFruit[height][width], int& remainingFruits, std::vector<Fruit>& fruits) {
    int nextX = x;
    int nextY = y;

    switch (dir) {
    case LEFT:  nextX--; break;
    case RIGHT: nextX++; break;
    case UP:    nextY--; break;
    case DOWN:  nextY++; break;
    default:    break;
    }

    // Sprawdzenie zderzenia z map¹
    if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height && map[nextY][nextX] != '#') {
        x = nextX;
        y = nextY;

        // Sprawdzenie zjedzenia owocu
        for (auto it = fruits.begin(); it != fruits.end(); ++it) {
            if (it->x == x && it->y == y) {
                if (it->isSpecial) {
                    powerUpActive = true;
                    powerUpTimer = eat_time;
                }
                fruits.erase(it); // Usuñ owoc z listy
                break;
            }
            if (hasFruit[y][x]) {
                score += 1;
                hasFruit[y][x] = false;
                remainingFruits--;
            }
        }
    }

    // Odliczanie czasu trwania efektu specjalnego owocu
    if (powerUpActive) {
        powerUpTimer--;
        if (powerUpTimer == 0) {
            powerUpActive = false;
        }
    }
}

int PacMan::GetPowerUpTimeLeft() const {
    return powerUpTimer;
}
