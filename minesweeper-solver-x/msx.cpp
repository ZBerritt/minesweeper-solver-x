#include <iostream>
#include <chrono>
#include <thread>
#include "solver.h"
#include "game.h"

int main() {
    printf("Searching for game...");
    std::unique_ptr<Game> game = find_game();
    while (game == nullptr) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        game = find_game();
    }
    Solver solver = Solver(game->get_board());
    while (game->status() == IN_PROGRESS) {
        game->get_board()->print();
        std::vector<Move> moves = solver.get_moves();
        for (Move move : moves) {
            if (move.action == FLAG_ACTION) {
                game->flag(move.x, move.y);
            }
            else {
                game->click(move.x, move.y);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        game->update();
    }

    if (game->status() == WON) {
        std::cout << "I win!" << std::endl;
    }
    else if (game->status() == LOST) {
        std::cout << "Game over." << std::endl;
    }

    return 0;
}
