#include "Controller.h"
#include <conio.h> // Dla _kbhit()

void Controller::GetInput(PacMan& pacman1, PacMan& pacman2) {
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
