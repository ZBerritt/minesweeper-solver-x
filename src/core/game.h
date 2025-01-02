#pragma once
#include <chrono>
#include "board.h"

enum Status {
    IN_PROGRESS,
    LOST,
    WON
};

// Abstract class representing a Minesweeper game
class Game {
public:
    static std::unique_ptr<Game> get_game(std::string type, const std::chrono::milliseconds& delay_override); // Game factory
    virtual Status status() = 0;
    virtual void update() = 0;
    virtual void click(int x, int y) = 0;
    virtual void flag(int x, int y) = 0;
    virtual int get_failed_cycle_threshold() = 0;
    std::shared_ptr<Board> get_board() const { return board; }
    std::chrono::milliseconds get_move_delay() const { return move_delay; }

protected:
    Game(std::string n, int w, int h, std::chrono::milliseconds md) : name(n), width(w), height(h),
        move_delay(md), board(std::make_shared<Board>(w, h)) {}
    std::string name;
    int width;
    int height;
    std::chrono::milliseconds move_delay;
    std::shared_ptr<Board> board;
};