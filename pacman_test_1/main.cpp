//customowe biblioteki
#include "Fruit.h"
#include "Entity.h"
#include "Constants.h"
#include "PacMan.h"
#include "Ghost.h"
#include "Controller.h"
#include "ClientHandler.h"
#include "Game.h"


int main() {
    //główny propgram gry
    SetConsoleTitleA("Pacman");
    srand(time(0));

    bool isSinglePlayer = true;
    bool isSimpleMap = true;
    bool ispodglad_gry = false;
    Difficulty difficultyLevel = EASY;

    Game game(isSinglePlayer, isSimpleMap, difficultyLevel, ispodglad_gry);
    game.ShowMenu(isSinglePlayer, isSimpleMap, ispodglad_gry);

    
    if (ispodglad_gry == false) {
        Server server;
        if (server.start()) {
            server.acceptClient(game);
            std::string command = "start \"\" \"C:\\Users\\Oskar\\source\\repos\\klient1\\klient\\x64\\Debug\\klient.exe\"";
            // Uruchomienie aplikacji w nowym oknie konsoli
            int result = std::system(command.c_str());

        }   
    }
    
    game = Game(isSinglePlayer, isSimpleMap, difficultyLevel, ispodglad_gry);

    game.Run();
    return 0;
}

