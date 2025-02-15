#pragma once
#include <memory>
#include <array>
#include "core/game.h"
#include "utils/screen.h"

class Google : public Game {
public:
    static std::unique_ptr<Google> find_game();
    Google(const Position& pos, const Dimension& board_dimensions, const Dimension& box_dimensions);
    Status status() override;
    void update() override;
    void click(int x, int y) override;
    void flag(int x, int y) override;
	int get_failed_cycle_threshold() override { return 4; }

private:
    // Cache frequently used values
    const Position position;
    const Dimension board_dimensions;
    const Dimension box_dimensions;

    // Screen object used
    Screen screen;

    // Helper methods
    Position box_mouse_position(int x, int y) const;
    int tile_value(int x, int y) const;
    std::pair<Position, Dimension> tile_range(int x, int y) const;

    static int classify_pixel(const Pixel& pixel);
    std::vector<Pixel> generate_sample_points(int x, int y) const;
};

const int TILE_VALUE_COUNT = 7; // # of tile values (0-6)

// Board Colors
const Pixel LIGHT_UND{ 170, 215, 81 };
const Pixel DARK_UND{ 162, 209, 73 };
const Pixel LIGHT_EMPTY{ 224, 195, 163 };
const Pixel DARK_EMPTY{ 211, 185, 157 };
const Pixel BORDER{ 126, 164, 53 };
//const Pixel FLAG{ 242, 54, 7 };
const Pixel RESULTS{ 77, 193, 249 };
const Pixel NUM_ONE{ 25, 118, 210 };
const Pixel NUM_TWO{ 55, 141, 59 };
const Pixel NUM_THREE{ 211, 47, 47 };
const Pixel NUM_FOUR{ 123, 31, 162 };
const Pixel NUM_FIVE{ 255, 139, 0 };
const Pixel NUM_SIX{ 30, 157, 169 };

// Pixel classification
const std::array<std::pair<Pixel, int>, 11> pixel_classification = { {
        {LIGHT_UND, UNDISCOVERED},
        {DARK_UND, UNDISCOVERED},
        {LIGHT_EMPTY, 0},
        {DARK_EMPTY, 0},
        //{FLAG, MINE},
        {NUM_ONE, 1},
        {NUM_TWO, 2},
        {NUM_THREE, 3},
        {NUM_FOUR, 4},
        {NUM_FIVE, 5},
        {NUM_SIX, 6}
    } };