#ifndef ENTITY_H
#define ENTITY_H

class Entity {
public:
    int x, y;

    Entity(int startX, int startY) : x(startX), y(startY) {}
    virtual ~Entity() = default;

    // Mo¿esz dodaæ inne metody, które bêd¹ wspólne dla wszystkich encji
};

#endif // ENTITY_H
