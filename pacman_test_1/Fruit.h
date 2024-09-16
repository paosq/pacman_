#ifndef FRUIT_H
#define FRUIT_H

#include <vector>
#include "Entity.h"
#include "Constants.h"

class Fruit : public Entity {
    public:
        Fruit(int startX, int startY, bool special = false) : Entity(startX, startY), isSpecial(special) {}
        bool isSpecial;
    
    private:
        void Respawn(char map[height][width], std::vector<Fruit>& fruits, bool hasFruit[height][width]);
};

#endif // FRUIT_H
