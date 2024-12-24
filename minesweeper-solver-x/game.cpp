#include <cmath>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>

#include "game.h"

static bool color_in_range(const Pixel& a, const Pixel& b, int range = 10) {
    return abs(a.red - b.red) <= range &&
        abs(a.green - b.green) <= range &&
        abs(a.blue - b.blue) <= range;
}

static Dimension find_box_dimensions(Screen& screen, const Position& top_left) {
    for (uint32_t y = top_left.y + 1; y < screen.get_dimension().height; y++) {
        Pixel pixel = screen.get_pixel(top_left.x, y);
        if (color_in_range(pixel, DARK_UND)) {
            for (uint32_t x = top_left.x + 1; x < screen.get_dimension().width; x++) {
                pixel = screen.get_pixel(x, y - 1);
                if (color_in_range(pixel, DARK_UND)) {
                    return Dimension(
                        static_cast<uint32_t>(x - top_left.x + 2),
                        static_cast<uint32_t>(y - top_left.y + 2)
                    );
                }
            }
        }
    }
    return Dimension(0, 0);
}

static Dimension find_board_dimensions(Screen& screen, const Position& top_left) {
    for (uint32_t y = top_left.y + 1; y < screen.get_dimension().height; y++) {
        Pixel pixel = screen.get_pixel(top_left.x, y);
        if (!color_in_range(pixel, LIGHT_UND) && !color_in_range(pixel, DARK_UND)) {
            for (uint32_t x = top_left.x + 1; x < screen.get_dimension().width; x++) {
                pixel = screen.get_pixel(x, y - 1);
                if (!color_in_range(pixel, LIGHT_UND) && !color_in_range(pixel, DARK_UND)) {
                    return Dimension(
                        static_cast<uint32_t>(x - top_left.x),
                        static_cast<uint32_t>(y - top_left.y)
                    );
                }
            }
        }
    }
    return Dimension(0, 0);
}

Game::Game(const Position& pos, const Dimension& board_dim, const Dimension& box_dim)
    : position(pos)
    , board_dimensions(board_dim)
    , box_dimensions(box_dim)
    , screen(pos, board_dim)
    , board(std::make_shared<Board>(board_dim.width / box_dim.width, board_dim.height / box_dim.height))
{
    update();
}

// Assumes the board's screenshot has already been taken
Status Game::status() {
    // Use pointer arithmetic for faster iteration
    const Pixel* first_pixel = &screen.get_pixel(0, 0);
    const size_t total_pixels = board_dimensions.width * board_dimensions.height;

    // Check for game over condition
    for (size_t i = 0; i < total_pixels; ++i) {
        if (color_in_range(first_pixel[i], RESULTS)) {
            return LOST;
        }
    }

    // Check win condition
    std::vector<Tile> undiscovered = board->get_undiscovered_tiles();
    if (undiscovered.empty()) {
        return WON;
    }
    return IN_PROGRESS;
}

void Game::click(int x, int y) {
    const Position pos = box_mouse_position(x, y);
    move_mouse(pos);
    left_click();
}

void Game::flag(int x, int y) {
    const Position pos = box_mouse_position(x, y);
    move_mouse(pos);
    right_click();
}

void Game::update() {
    move_mouse({ 0, 0 }); // Move mouse out of the way of the game board
    screen.take_screenshot();

    std::vector<Tile> tiles = board->get_undiscovered_tiles();
    for (const Tile& tile : tiles) {
        int value = tile_value(tile.x, tile.y);
        board->set_tile(tile.x, tile.y, value);
    }
}

int Game::tile_value(int x, int y) const {
    const Position mid = box_mouse_position(x, y);
    const int mid_x = mid.x - position.x;
    const int mid_y = mid.y - position.y;
    const Pixel& pixel = screen.get_pixel(mid_x, mid_y);
    int pixel_value = classify_pixel(pixel);
    if (pixel_value != UNKNOWN && pixel_value != 0) { // Ignore false positives, numbered tile has empty colors
        return pixel_value;
    }
    std::pair<Position, Dimension> range = tile_range(x, y);
    for (uint32_t i = range.first.x; i < range.first.x + range.second.width; i++) {
        for (uint32_t j = range.first.y; j < range.first.y + range.second.height; j++) {
            pixel_value = classify_pixel(screen.get_pixel(i, j));
            if (pixel_value != UNKNOWN && pixel_value != 0) {
                return pixel_value;
            }
        }
    }
    return pixel_value;
}

// Relative to the computer screen
Position Game::box_mouse_position(int x, int y) const {
    return Position(
        position.x + (box_dimensions.width * x) + box_dimensions.width / 2,
        position.y + (box_dimensions.height * y) + box_dimensions.height / 2
    );
}


// Relative to the screen object
std::pair<Position, Dimension> Game::tile_range(int x, int y) const {
    const uint32_t left_x = box_dimensions.width * x;
    const uint32_t top_y = box_dimensions.height * y;
    const uint32_t right_x = left_x + box_dimensions.width - 1;
    const uint32_t bottom_y = top_y + box_dimensions.height - 1;

    return std::make_pair(Position(left_x, top_y), Dimension(right_x - left_x + 1, bottom_y - top_y + 1));
}

int Game::classify_pixel(const Pixel& pixel) {
    // Iterate through the mapping to find a matching color range
    for (const auto& [range, value] : pixel_classification) {
        if (color_in_range(pixel, range)) {
            return value;
        }
    }

    // Return UNKNOWN if no match is found
    return UNKNOWN;
}

std::unique_ptr<Game> Game::find_game() {
    Screen screen = Screen();
    screen.take_screenshot();

    const uint32_t width = screen.get_dimension().width;
    const uint32_t height = screen.get_dimension().height;

    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            if (color_in_range(screen.get_pixel(x, y), LIGHT_UND)) {
                const Position position(x, y);

                const Dimension box_dimensions = find_box_dimensions(screen, position);
                const Dimension board_dimensions = find_board_dimensions(screen, position);

                if (box_dimensions.width > 0 && box_dimensions.height > 0 &&
                    board_dimensions.width > 0 && board_dimensions.height > 0) {
                    return std::make_unique<Game>(position, board_dimensions, box_dimensions);
                }
            }
        }
    }

    return nullptr;
}