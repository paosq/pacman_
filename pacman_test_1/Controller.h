#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "PacMan.h" // Upewnij siê, ¿e PacMan jest zaimportowany

class Controller {
public:
    void GetInput(PacMan& pacman1, PacMan& pacman2);
};

#endif // CONTROLLER_H
