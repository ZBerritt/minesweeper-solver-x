#pragma once

#include <memory>
#include "screen.h"
#include "board.h"

enum Status {
    IN_PROGRESS,
    LOST,
    WON
};

class Game {
    public:
        Game(std::shared_ptr<Position> pos,std::shared_ptr<Dimension> board_dimensions,std::shared_ptr<Dimension> box_dimensions);
        std::shared_ptr<Board> get_board();
        Status status();
        void update();
        void click(int x, int y);
        void flag(int x, int y);

    private:
        std::shared_ptr<Position> position;
        std::shared_ptr<Dimension> board_dimensions;
        std::shared_ptr<Dimension> box_dimensions;
        std::shared_ptr<Board> board;
        Position box_mouse_position(int x, int y);
        int tile_value(int x, int y, const Screenshot& screen);
        std::pair<Position, Dimension> tile_range(int x, int y);
};

std::unique_ptr<Game> find_game();
