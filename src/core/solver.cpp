#include <vector>

#include "solver.h"

Solver::Solver(std::shared_ptr<Board> b) : board(b) {}

std::set<Move> Solver::get_moves(bool guess) {
    int discovered = board->discovered_count();
    if (discovered == 0) {
        return guess ? first_move() : std::set<Move>{};
    }
    else if (discovered == board->get_width() * board->get_height()) {
        return std::set<Move>{};
    }

    std::set<Move> moves = basic_move();
    if (moves.empty()) {
        moves = guess ? guess_move() : std::set<Move>{};
    }
    return moves;
}

std::set<Move> Solver::first_move() {
    std::vector<Tile> undiscovered = board->get_undiscovered_tiles();
    int mid_index = (board->get_height() / 2 * board->get_width()) + (board->get_width() / 2);
    return { { CLICK_ACTION, undiscovered[mid_index].x, undiscovered[mid_index].y } };
}


std::set<Move> Solver::basic_move() {
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


std::set<Move> Solver::guess_move() {
    const std::vector<Tile>& undiscovered = board->get_undiscovered_tiles();
    if (undiscovered.empty()) {
        return std::set<Move>();
    }

    Tile best_tile = undiscovered[0];
    double best_score = -1;

    // Pre-allocate vectors to avoid repeated allocations
    std::vector<Tile> surrounding;
    std::vector<Tile> neighbor_tiles;
    surrounding.reserve(8);
    neighbor_tiles.reserve(8);

    // Cache board dimensions to avoid repeated function calls
    const int board_width = board->get_width();
    const int board_height = board->get_height();

    for (const Tile& t : undiscovered) {
        double tile_score = 0.0;
        int valid_neighbors = 0;

        // Clear vectors instead of reallocating
        surrounding.clear();

        // Manually inline get_surrounding_tiles logic for better performance
        const int start_i = std::max(0, t.y - 1);
        const int end_i = std::min(board_height - 1, t.y + 1);
        const int start_j = std::max(0, t.x - 1);
        const int end_j = std::min(board_width - 1, t.x + 1);

        for (int i = start_i; i <= end_i; i++) {
            for (int j = start_j; j <= end_j; j++) {
                surrounding.push_back(board->get_tile(j, i));
            }
        }

        for (const Tile& s : surrounding) {
            // Only consider numbered tiles
            if (s.value > 0) {
                neighbor_tiles.clear();

                // Manually inline get_surrounding_tiles again
                const int n_start_i = std::max(0, s.y - 1);
                const int n_end_i = std::min(board_height - 1, s.y + 1);
                const int n_start_j = std::max(0, s.x - 1);
                const int n_end_j = std::min(board_width - 1, s.x + 1);

                int undiscovered_count = 0;
                int remaining_mines = s.value;

                // Count undiscovered tiles and remaining mines in one pass
                for (int i = n_start_i; i <= n_end_i; i++) {
                    for (int j = n_start_j; j <= n_end_j; j++) {
                        const Tile& neighbor = board->get_tile(j, i);
                        if (neighbor.value == UNDISCOVERED) {
                            undiscovered_count++;
                        }
                        else if (neighbor.value == MINE) {
                            remaining_mines--;
                        }
                    }
                }

                if (undiscovered_count > 0) {
                    // Calculate safety score: lower ratio of mines to undiscovered = safer
                    tile_score += 1.0 - (static_cast<double>(remaining_mines) / undiscovered_count);
                    valid_neighbors++;
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