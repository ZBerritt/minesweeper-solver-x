#pragma once
#include <memory>
#include "game.h"

enum Action {
    CLICK_ACTION,
    FLAG_ACTION
};

enum SolverResult {
    SUCCESS,
    FAILURE,
    STUCK
};

struct Move {
    Action action;
    int x;
    int y;

    // For set operations
    bool operator<(const Move& other) const {
        if (action != other.action)
            return action < other.action;
        if (x != other.x)
            return x < other.x;
        return y < other.y;
    }
};

class Solver {
public:
    Solver(std::shared_ptr<Game> g, bool v, bool pb);
    SolverResult solve();

private:
    std::shared_ptr<Game> game;
	bool verbose;
	bool print_board;
};