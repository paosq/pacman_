#include "Fruit.h"
#include <cstdlib>


void Fruit::Respawn(char map[height][width], std::vector<Fruit>& fruits, bool hasFruit[height][width]) {
    int newX, newY;
    bool special;

    do {
        newX = 1 + rand() % (width - 2); // Unikamy umieszczania owoców na krawêdziach
        newY = 1 + rand() % (height - 2);
        special = (rand() % 5 == 0); // 1 na 5

    } while (map[newY][newX] == '#' || hasFruit[newY][newX]); // Jeœli owoc nie mo¿e byæ na tym miejscu

    // Tworzenie nowego owocu i dodawanie do wektora
    fruits.push_back(Fruit(newX, newY, special));
    hasFruit[newY][newX] = true;
}
