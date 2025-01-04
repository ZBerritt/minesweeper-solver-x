#include <vector>
#include <chrono>
#include <thread>
#include <set>
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

    // Cache board dimensions
    const int board_width = board->get_width();
    const int board_height = board->get_height();

    Tile best_tile = undiscovered[0];
    double lowest_mine_probability = 1.0;
    int best_valid_neighbors = 0;

    // Process each undiscovered tile
    for (const Tile& t : undiscovered) {
        double cumulative_prob = 0.0;
        int valid_neighbors = 0;
        bool has_info = false;

        // Get surrounding tiles efficiently using the template function
        const std::vector<Tile>& surrounding = surrounding_tiles<Tile>(t.x, t.y, board_width, board_height,
            [&](int x, int y) { return board->get_tile(x, y); });

        // Analyze each surrounding numbered tile
        for (const Tile& s : surrounding) {
            if (s.value > 0) {  // Only consider numbered tiles
                const std::vector<Tile>& s_neighbors = surrounding_tiles<Tile>(s.x, s.y, board_width, board_height,
                    [&](int x, int y) { return board->get_tile(x, y); });

                int undiscovered_count = 0;
                bool valid_calculation = true;
                int remaining_mines = board->remaining_nearby_mines(s);

                // Count undiscovered neighbors
                for (const Tile& n : s_neighbors) {
                    if (n.value == UNKNOWN) {
                        valid_calculation = false;
                        break;
                    }
                    if (n.value == UNDISCOVERED) {
                        undiscovered_count++;
                    }
                }

                // Calculate probability contribution from this numbered tile
                if (valid_calculation && undiscovered_count > 0) {
                    double local_prob = static_cast<double>(remaining_mines) / undiscovered_count;
                    cumulative_prob += local_prob;
                    valid_neighbors++;
                    has_info = true;
                }
            }
        }

        // Calculate final probability for this tile
        if (has_info && valid_neighbors > 0) {
            double avg_probability = cumulative_prob / valid_neighbors;

            // Apply neighbor bonus (weighted less heavily than in original)
            double neighbor_bonus = 1.0 + (valid_neighbors / 12.0);
            double final_probability = avg_probability / neighbor_bonus;

            // Update best tile if this one has lower probability
            if (final_probability < lowest_mine_probability ||
                (final_probability == lowest_mine_probability && valid_neighbors > best_valid_neighbors)) {
                lowest_mine_probability = final_probability;
                best_tile = t;
                best_valid_neighbors = valid_neighbors;
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

Solver::Solver(std::shared_ptr<Game> g, bool v) : game(g), display(v ? std::make_shared<BoardDisplay>(g->get_board()) : nullptr) {}

SolverResult Solver::solve() {
	const int failed_cycle_threshould = game->get_failed_cycle_threshold(); // Number of failed cycles before guessing
	const std::shared_ptr<Board> board = game->get_board();
    int failed_cycles = 0;
    bool guessing = true;

    while (game->status() == IN_PROGRESS) {
        update_board();
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
				print_move(move.x, move.y, move.action);
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

	update_board();

	return game->status() == WON ? SUCCESS : FAILURE;
}

void Solver::update_board() {
    if (display != nullptr) {
		display->update_board();
    }
}

void Solver::print_move(int x, int y, Action action) {
    if (display != nullptr) {
        display->print(std::string(action == CLICK_ACTION ? "Click" : "Flag")
            + ": (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
}