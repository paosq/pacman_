#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr int PORT = 1234;
constexpr int BUFFER_SIZE = 1024;

const int width = 40;
const int height = 20;
const int game_speed = 300;
const int eat_time = 50;// Ustawienie czasu trwania mocy

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum Difficulty { EASY, MEDIUM, HARD };

#endif // CONSTANTS_H
