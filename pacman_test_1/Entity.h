#ifndef ENTITY_H
#define ENTITY_H

class Entity {
public:
    int x, y;

    Entity(int startX, int startY) : x(startX), y(startY) {}
    virtual ~Entity() = default;

    // Mo�esz doda� inne metody, kt�re b�d� wsp�lne dla wszystkich encji
};

#endif // ENTITY_H
