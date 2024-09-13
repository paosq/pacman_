#ifndef PACMAN_H
#define PACMAN_H

#include <vector>
#include <iostream>
#include <conio.h> 
#include "Entity.h"
#include "Fruit.h" // Importujemy Fruit, je�li PacMan u�ywa tej klasy
#include "Constants.h"

class PacMan : public Entity {
public:
    Direction dir;
    char symbol;
    int score;
    int lives; // liczba �y�
    bool powerUpActive; // Okre�la, czy PacMan mo�e zjada� duchy
    int powerUpTimer;   // Czas trwania efektu zjedzenia specjalnego owocu
    std::vector<Fruit> fruits;

    PacMan(int startX, int startY, char sym);

    void Move(char map[height][width], bool hasFruit[height][width], int& remainingFruits, std::vector<Fruit>& fruits);
    int GetPowerUpTimeLeft() const;
};

#endif // PACMAN_H
