#pragma once
#include "board.h"
#include <set>
#include <memory>

enum Action {
    CLICK_ACTION,
    FLAG_ACTION
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
    Solver(std::shared_ptr<Board> b);
    std::set<Move> get_moves(); 

private:
    std::shared_ptr<Board> board;
    std::set<Move> random_move();
    std::set<Move> basic_move();
    std::set<Move> guess_move();
};