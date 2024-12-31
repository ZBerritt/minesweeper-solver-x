#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include "core/game.h"
#include "benchmarks/bench.h"
#include "core/solver.h"

static const std::string HELP_MESSAGE = "Minesweeper Solver X [Version 1.0.0]\nUsage: msx [-hbpvd] {google,veasy,vmedium,vhard,vimpossible}";

int main(int argc, char* argv[]) {
    // Options
    std::chrono::milliseconds delay_override{};
    std::string game_type;
    bool print_board = false;
    bool verbose = false;
    for (int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if (arg.at(0) == '-') {
            for (int j = 1; j < arg.size(); j++) {
                if (arg.at(j) == 'b') {
                    Benchmark::full_benchmark();
                    return 0;
                }
                else if (arg.at(j) == 'h') {
                    std::cout << HELP_MESSAGE << std::endl;
                    return 0;
                }
                else if (arg.at(j) == 'p') {
                    print_board = true;
                }
                else if (arg.at(j) == 'v') {
                    verbose = true;
                }
                else if (arg.at(j) == 'd') {
                    if (i + 1 >= argc) {
                        std::cerr << "Must specify a delay in milliseconds" << std::endl;
                        return -1;
                    }
                    delay_override = std::chrono::milliseconds(stoi(std::string(argv[i + 1])));
                    i++;
                }
                else {
                    std::cerr << "Unknown argument -" << arg.at(j) << std::endl;
                    std::cout << HELP_MESSAGE << std::endl;
                    return -1;
                }
            }
        }
        else {
            game_type = std::string(argv[i]);
        }
    }

    if (game_type.empty()) {
        std::cout << HELP_MESSAGE << std::endl;
        return 0;
    }

    // Get correct game
    std::unique_ptr<Game> game = Game::get_game(game_type, delay_override);
    if (!game) {
       std::cerr << "Invalid game type " << game_type << std::endl;
       return -1;
    }

    // Execute solver
    std::cout << "Starting Minesweeper Solver X for game type " << game_type << std::endl;
    Solver solver(game->get_board());
    while (game->status() == IN_PROGRESS) {
        if (print_board) {
            game->get_board()->print();
        }
        std::set<Move> moves = solver.get_moves();
		if (moves.empty()) {
			break;
		}
        for (Move move : moves) {
            if (verbose) {
                std::cout << (move.action == CLICK_ACTION ? "Click" : "Flag") << ": (" << std::to_string(move.x) << ", " << std::to_string(move.y) << ")" << std::endl;
            }
            if (move.action == FLAG_ACTION) {
				game->get_board()->set_tile(move.x, move.y, MINE); // Manually set as mine for algorithm
				//game->flag(move.x, move.y); // Commented out to prevent flagging
            }
            else if (move.action == CLICK_ACTION) {
                game->click(move.x, move.y);
            }
        }
        std::this_thread::sleep_for(game->get_move_delay());
        game->update();
    }

    if (print_board) {
        game->get_board()->print();
    }

    // Ending message
	if (game->status() == IN_PROGRESS) {
		std::cout << "I'm stuck..." << std::endl;
	} else if (game->status() == WON) {
        std::cout << "I win!" << std::endl;
    }
    else if (game->status() == LOST) {
        std::cout << "Game over." << std::endl;
    }

    return 0;
}
