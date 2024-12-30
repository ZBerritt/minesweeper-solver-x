#include <numeric>

#include "solver.h"

Solver::Solver(std::shared_ptr<Board> b) : board(b) {}


std::vector<Move> Solver::get_moves() {
   if (board->discovered_count() == 0) {
       return random_move();
   }

   std::vector<Move> moves = basic_move();
   if (moves.empty()) {
	   moves = guess_move();
   }
   return moves;
}

std::vector<Move> Solver::random_move() {
    std::vector<Move> moves;
	std::vector<Tile> undiscovered = board->get_undiscovered_tiles();
    int random = (rand() % undiscovered.size());
	moves.push_back({ CLICK_ACTION, undiscovered[random].x, undiscovered[random].y });
    return moves;
}


std::vector<Move> Solver::basic_move() {
	std::vector<Move> moves;
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
					moves.push_back({ FLAG_ACTION, s.x, s.y });
				}
				if (mine_chance == 0) {
					moves.push_back({ CLICK_ACTION, s.x, s.y });
				}
			}
		}
	}
	return moves;
}


std::vector<Move> Solver::guess_move() {
	Tile* best_tile = nullptr;
	double best_prob = -1;
	std::vector<Tile> undiscovered = board->get_undiscovered_tiles();
	for (Tile t : undiscovered) {
		std::vector<Tile> surrounding = board->get_surrounding_tiles(t);
		std::vector<double> probs;
		for (Tile s : surrounding) {
			int surrounding_mines = board->remaining_nearby_mines(s);
			if (surrounding_mines > 0) {
				probs.push_back(1.0 / surrounding_mines);
			}
		}
		double avg_prob = 0;
		if (!probs.empty()) {
			avg_prob = std::accumulate(probs.begin(), probs.end(), 0.0) / probs.size();
		}
		if (avg_prob > best_prob) {
			best_tile = &t;
			best_prob = avg_prob;
		}
	}

	if (best_tile) {
		return { { CLICK_ACTION, best_tile->x, best_tile->y } };
	}
	else {
		return random_move();
	}
}


/*def prob_algorithm(board: Board) -> set:
    best_tile = None
    best_prob = -1
    tiles = board.get_undiscovered_borders()
    for tile in tiles:
        probs = []
        for sur_tile in board.get_surrounding_tiles(tile):
            surrounding_mines = board.remaining_nearby_mines(sur_tile)
            if surrounding_mines > 0:
                probs.append(1 / surrounding_mines)
        avg_prob = np.average(probs)
        if avg_prob > best_prob:    
            best_tile = tile
            best_prob = avg_prob

    return set([(best_tile.x, best_tile.y, Action.CLICK)]) if best_tile else get_random_move(board)*/