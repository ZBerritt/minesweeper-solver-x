#include <iostream>
#include <chrono>
#include <thread>
#include "benchmarks/bench.h"
#include "core/solver.h"
#include "games/google.h"

int main(int argc, char* argv[]) {
    // Options
    for (int i = 0; i < argc; i++) {
        if (std::string(argv[i]) == "-b") {
            Benchmark::full_benchmark();
            return 0;
        }
        else if (std::string(argv[i]) == "-h") {
            std::cout << "Minesweeper Solver X [Version 1.0.0]" << std::endl
                << "Usage: msx [-hb]" << std::endl;
            return 0;
        }
    }

    // Search
    std::cout << "Searching for game..." << std::endl;
    std::unique_ptr<Google> game = Google::find_game();

    // Start
	std::cout << "Found game! Beginning solver..." << std::endl;
    Solver solver(game->get_board());
    while (game->status() == G_IN_PROGRESS) {
        //game->get_board()->print();
        std::set<Move> moves = solver.get_moves();
		if (moves.empty()) {
			break;
		}
        for (Move move : moves) {
            if (move.action == FLAG_ACTION) {
				game->get_board()->set_tile(move.x, move.y, MINE); // Manually set as mine for algorithm
				//game->flag(move.x, move.y); // Commented out to prevent flagging
            }
            else if (move.action == CLICK_ACTION) {
                game->click(move.x, move.y);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(850));
        game->update();
    }

    // Ending message
	if (game->status() == G_IN_PROGRESS) {
		std::cout << "I'm stuck..." << std::endl;
	} else if (game->status() == G_WON) {
        std::cout << "I win!" << std::endl;
    }
    else if (game->status() == G_LOST) {
        std::cout << "Game over." << std::endl;
    }

    return 0;
}
