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
    std::vector<Move> random_move();
    std::vector<Move> basic_move();
    std::vector<Move> guess_move();
};