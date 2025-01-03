#include <iostream>
#include <chrono>
#include <string>
#include "core/game.h"
#include "benchmarks/bench.h"
#include "core/solver.h"

namespace {
    constexpr std::string_view HELP_MESSAGE = "Minesweeper Solver X [Version 1.0.0]\nUsage: msx [-hvd] {google,veasy,vmedium,vhard,vimpossible} | msx -b[v]";
    
    struct ProgramOptions {
		bool benchmark = false;
		bool verbose = false;
		bool print_board = false;
        std::chrono::milliseconds delay_override{};
		std::string game_type;
    };

    ProgramOptions arg_parse(int argc, char* argv[]) {
        ProgramOptions options;


        for (int i = 1; i < argc; i++) {
            std::string_view arg = std::string_view(argv[i]);
            if (arg.empty()) continue; 

            if (arg[0] == '-') {
                for (size_t j = 1; j < arg.length(); j++) {
                    switch (arg[j]) {
                        case 'b':
							options.benchmark = true;
                            break;
					    case 'h':
						    std::cout << HELP_MESSAGE << std::endl;
						    std::exit(0);
					    case 'v':
						    options.verbose = true;
						    break;
					    case 'd':
                            if (i + 1 >= argc) {
                                throw std::runtime_error("Must specify a delay in milliseconds");
                            }
                            options.delay_override = std::chrono::milliseconds(std::stoi(argv[++i]));
						    break;
					    default:
                            throw std::runtime_error(
                                std::string("Unknown argument -") + arg[j]);
                    }
                }
			}
			else {
				options.game_type = std::string(argv[i]);
			}
        }

        if (options.game_type.empty() && !options.benchmark) {
            throw std::runtime_error("Game type must be specified");
        }

        return options;
    }
}



int main(int argc, char* argv[]) {
    try {
        ProgramOptions options = arg_parse(argc, argv);
        if (options.benchmark) {
			Benchmark::full_benchmark(options.verbose);
			return 0;
        }

        // Get correct game
        std::shared_ptr<Game> game = Game::get_game(options.game_type, options.delay_override);
        if (!game) {
            throw std::runtime_error("Invalid game type: " + options.game_type);
        }

        // Execute solver
        std::cout << "Starting Minesweeper Solver X for game type " << options.game_type << std::endl;
        Solver solver = Solver(game, options.verbose);
        SolverResult result = solver.solve();

        // Ending message
        switch (result) {
            case SUCCESS:
                std::cout << "I won!" << std::endl;
                break;
            case FAILURE:
                std::cout << "I lost..." << std::endl;
                break;
            case STUCK:
                std::cout << "I'm stuck..." << std::endl;
                break;
            }
        return 0;
    }
    catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cout << HELP_MESSAGE << std::endl;
        return 1;
    }
}
