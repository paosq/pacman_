#ifndef GHOST_H
#define GHOST_H

#include "Entity.h"
#include "PacMan.h"  // Dodaj, je�li `PacMan` jest u�ywany w metodach klasy `Ghost`
#include "Constants.h"

class Ghost : public Entity {
public:
    Direction dir;
    char symbol;
    int speed; // szybko�� duch�w w zale�no�ci od poziomu
    bool isg;  // decyduje, czy duch jest ma�ym 'g' czy du�ym 'G' w zale�no�ci od super owocu

    Ghost(int startX, int startY, char sym, int spd);

    void Move(char map[height][width], const PacMan& pacman);
    void setisg(bool isPowerUpActive);
};

#endif // GHOST_H
