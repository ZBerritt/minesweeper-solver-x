#pragma once
#include "board.h"
#include <vector>
#include <memory>

enum Action {
    CLICK_ACTION,
    FLAG_ACTION
};

struct Move {
    Action action;
    int x;
    int y;
};

class Solver {
    public:
        Solver(std::shared_ptr<Board> b);
        std::vector<Move> get_moves(); 

    private:
        std::shared_ptr<Board> board;
};