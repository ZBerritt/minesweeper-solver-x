#include "solver.h"
#include <memory>

Solver::Solver(std::shared_ptr<Board> b) {
    this->board = b;
}

std::vector<Move> Solver::get_moves() {
   std::vector<Move> moves;

    // Example implementation: Generate some dummy moves
   std::vector<Tile> tiles = board->get_undiscovered_tiles();
   moves.push_back({CLICK_ACTION, tiles[0].x, tiles[0].y});

    return moves;
}
