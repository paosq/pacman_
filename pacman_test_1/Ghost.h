#ifndef GHOST_H
#define GHOST_H

#include "Entity.h"
#include "PacMan.h"  // Dodaj, jeœli `PacMan` jest u¿ywany w metodach klasy `Ghost`
#include "Constants.h"

class Ghost : public Entity {
public:
    Direction dir;
    char symbol;
    int speed; // szybkoœæ duchów w zale¿noœci od poziomu
    bool isg;  // decyduje, czy duch jest ma³ym 'g' czy du¿ym 'G' w zale¿noœci od super owocu

    Ghost(int startX, int startY, char sym, int spd);

    void Move(char map[height][width], const PacMan& pacman);
    void setisg(bool isPowerUpActive);
};

#endif // GHOST_H
