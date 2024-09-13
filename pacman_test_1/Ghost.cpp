#include "Ghost.h"
#include "Constants.h"

Ghost::Ghost(int startX, int startY, char sym, int spd)
    : Entity(startX, startY), dir(STOP), symbol(sym), speed(spd), isg(false) {}

void Ghost::Move(char map[height][width], const PacMan& pacman) {
    int nextX = x;
    int nextY = y;

    // Wyliczanie dystansu od gracza
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

    // Sprawdzenie kolizji z map¹
    if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height && map[nextY][nextX] != '#') {
        x = nextX;
        y = nextY;
    }
}

void Ghost::setisg(bool isPowerUpActive) {
    symbol = isPowerUpActive ? 'g' : 'G';
}
