#include <vector>
#include <chrono>
#include <thread>
#include <set>
#include <iostream>
#include "utils/util.h"
#include "board.h"

#include "solver.h"

static std::set<Move> first_move(std::shared_ptr<Board> board) {
    std::vector<Tile> undiscovered = board->get_undiscovered_tiles();
    int mid_index = (board->get_height() / 2 * board->get_width()) + (board->get_width() / 2);
    return { { CLICK_ACTION, undiscovered[mid_index].x, undiscovered[mid_index].y } };
}

static std::set<Move> basic_move(std::shared_ptr<Board> board) {
    std::set<Move> moves;
    for (Tile t : board->get_border_tiles()) {
        int remaining_mines = board->remaining_nearby_mines(t);
        std::vector<Tile> surrounding = board->get_surrounding_tiles(t);
        int undiscovered_count = 0;
        for (Tile s : surrounding) {
            if (s.value == UNDISCOVERED) {
                undiscovered_count++;
            }
        }
        double mine_chance = static_cast<double>(remaining_mines) / undiscovered_count;
        for (Tile s : surrounding) {
            if (s.value == UNDISCOVERED) {
                if (mine_chance == 1) {
                    moves.insert({ FLAG_ACTION, s.x, s.y });
                }
                if (mine_chance == 0) {
                    moves.insert({ CLICK_ACTION, s.x, s.y });
                }
            }
        }
    }
    return moves;
}

static std::set<Move> guess_move(std::shared_ptr<Board> board) {
    const std::vector<Tile>& undiscovered = board->get_undiscovered_tiles();
    if (undiscovered.empty()) {
        return std::set<Move>();
    }

    Tile best_tile = undiscovered[0];
    double best_score = -1;

    // Cache board dimensions to avoid repeated function calls
    const int board_width = board->get_width();
    const int board_height = board->get_height();

    for (const Tile& t : undiscovered) {
        double tile_score = 0.0;
        int valid_neighbors = 0;

        // Clear vectors instead of reallocating
		const std::vector<Tile>& surrounding = surrounding_tiles<Tile>(t.x, t.y, board_width, board_height, [&](int x, int y) { return board->get_tile(x, y); });

        for (const Tile& s : surrounding) {
            // Only consider numbered tiles
            if (s.value > 0) {
                const std::vector<Tile>& neighbors = surrounding_tiles<Tile>(s.x, s.y, board_width, board_height, [&](int x, int y) {
                    return board->get_tile(x, y);
                    });

                int undiscovered_count = 0;
                int remaining_mines = s.value;

                for (const Tile& neighbor : neighbors) {
                    if (neighbor.value == UNDISCOVERED) {
                        undiscovered_count++;
                    }
                    else if (neighbor.value == MINE) {
                        remaining_mines--;
                    }
                }
            }
        }

        // Only consider tiles with valid numbered neighbors
        if (valid_neighbors > 0) {
            // Calculate final score with neighbor bonus
            const double final_score = (tile_score / valid_neighbors) * (1.0 + (valid_neighbors / 8.0));

            if (final_score > best_score) {
                best_tile = t;
                best_score = final_score;
            }
        }
    }

    return { { CLICK_ACTION, best_tile.x, best_tile.y } };
}

static std::set<Move> get_moves(std::shared_ptr<Board> board, bool guess) {
    int discovered = board->discovered_count();
    if (discovered == 0) {
        return guess ? first_move(board) : std::set<Move>{};
    }
    else if (discovered == board->get_width() * board->get_height()) {
        return std::set<Move>{};
    }

    std::set<Move> moves = basic_move(board);
    if (moves.empty()) {
        moves = guess ? guess_move(board) : std::set<Move>{};
    }
    return moves;
}

Solver::Solver(std::shared_ptr<Game> g, bool v, bool pb) : game(g), verbose(v), print_board(pb) {}

SolverResult Solver::solve() {
	const int failed_cycle_threshould = game->get_failed_cycle_threshold(); // Number of failed cycles before guessing
	const std::shared_ptr<Board> board = game->get_board();
    int failed_cycles = 0;
    bool guessing = true;

    while (game->status() == IN_PROGRESS) {
        if (print_board) {
            game->get_board()->print();
        }
        std::set<Move> moves = get_moves(board, guessing);
        if (moves.empty() && guessing) {
            return STUCK;
        }
        else if (moves.empty() && ++failed_cycles >= failed_cycle_threshould) {
            guessing = true;
            failed_cycles = 0;
        }
        else {
            guessing = false;
            for (Move move : moves) {
                if (verbose) {
                    std::cout << (move.action == CLICK_ACTION ? "Click" : "Flag") << ": (" << std::to_string(move.x) << ", " << std::to_string(move.y) << ")" << std::endl;
                }
                if (move.action == FLAG_ACTION) {
                    game->get_board()->set_tile(move.x, move.y, MINE); 
                    //game->flag(move.x, move.y); // Commented out for now
                }
                else if (move.action == CLICK_ACTION) {
                    game->click(move.x, move.y);
                }
            }
        }
        std::this_thread::sleep_for(game->get_move_delay());
        game->update();
    }

    if (print_board) {
        game->get_board()->print();
    }

	return game->status() == WON ? SUCCESS : FAILURE;
}