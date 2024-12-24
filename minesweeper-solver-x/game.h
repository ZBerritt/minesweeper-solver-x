#pragma once
#include <memory>
#include <array>
#include "screen.h"
#include "board.h"

enum Status {
    IN_PROGRESS,
    LOST,
    WON
};

class Game {
public:
    static std::unique_ptr<Game> find_game();
    Game(const Position& pos, const Dimension& board_dimensions, const Dimension& box_dimensions);
    std::shared_ptr<Board> get_board() const { return board; }
    Status status();
    void update();
    void click(int x, int y);
    void flag(int x, int y);

private:
    // Cache frequently used values
    const Position position;
    const Dimension board_dimensions;
    const Dimension box_dimensions;

    Screen screen;
    std::shared_ptr<Board> board;

    // Helper methods
    Position box_mouse_position(int x, int y) const;
    int tile_value(int x, int y) const;
    std::pair<Position, Dimension> tile_range(int x, int y) const;

    static int classify_pixel(const Pixel& pixel);
};


// Board Colors
const Pixel LIGHT_UND{ 170, 215, 81 };
const Pixel DARK_UND{ 162, 209, 73 };
const Pixel LIGHT_EMPTY{ 224, 195, 163 };
const Pixel DARK_EMPTY{ 211, 185, 157 };
const Pixel BORDER{ 126, 164, 53 };
const Pixel FLAG{ 242, 54, 7 };
const Pixel RESULTS{ 77, 193, 249 };
const Pixel NUM_ONE{ 25, 118, 210 };
const Pixel NUM_TWO{ 55, 141, 59 };
const Pixel NUM_THREE{ 211, 47, 47 };
const Pixel NUM_FOUR{ 123, 31, 162 };
const Pixel NUM_FIVE{ 255, 139, 0 };
const Pixel NUM_SIX{ 30, 157, 169 };

// Pixel classification
const std::array<std::pair<Pixel, int>, 12> pixel_classification = { {
        {LIGHT_UND, UNDISCOVERED},
        {DARK_UND, UNDISCOVERED},
        {LIGHT_EMPTY, 0},
        {DARK_EMPTY, 0},
        {FLAG, MINE},
        {NUM_ONE, 1},
        {NUM_TWO, 2},
        {NUM_THREE, 3},
        {NUM_FOUR, 4},
        {NUM_FIVE, 5},
        {NUM_SIX, 6}
    } };