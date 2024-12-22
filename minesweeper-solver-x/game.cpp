#include <cmath>

#include "game.h"

static bool color_in_range(const Pixel& a, const Pixel& b, int range = 5) {
    return abs(a.red - b.red) <= range && abs(a.green - b.green) <= range && abs(a.blue - b.blue) <= range;
}

static int classify_pixel(const Pixel& pixel) {
    if (color_in_range(pixel, FLAG)) return MINE;
    if (color_in_range(pixel, LIGHT_UND) || color_in_range(pixel, DARK_UND)) return UNDISCOVERED;
    if (color_in_range(pixel, LIGHT_EMPTY) || color_in_range(pixel, DARK_EMPTY)) return 0;
    if (color_in_range(pixel, NUM_ONE)) return 1;
    if (color_in_range(pixel, NUM_TWO)) return 2;
    if (color_in_range(pixel, NUM_THREE)) return 3;
    if (color_in_range(pixel, NUM_FOUR)) return 4;
    if (color_in_range(pixel, NUM_FIVE)) return 5;
    if (color_in_range(pixel, NUM_SIX)) return 6;
    return UNDISCOVERED;
}

static Dimension find_box_dimensions(const Screenshot& screen, const Position& top_left) {
    for (int y = top_left.y + 1; y < screen.dim.height; y++) {
        Pixel pixel = screen.pixels[y][top_left.x];
        if (color_in_range(pixel, DARK_UND)) {
            for (int x = top_left.x + 1; x < screen.dim.width; x++) {
                pixel = screen.pixels[y - 1][x];
                if (color_in_range(pixel, DARK_UND)) {
                    return {static_cast<uint16_t>(x - top_left.x + 2),
                        static_cast<uint16_t>(y - top_left.y + 2)};
                }
            }
        }
    }
    return {0, 0};
}

static Dimension find_board_dimensions(const Screenshot& screen, const Position& top_left) {
    for (int y = top_left.y + 1; y < screen.dim.height; y++) {
        if (!color_in_range(screen.pixels[y][top_left.x], LIGHT_UND) && !color_in_range(screen.pixels[y][top_left.x], DARK_UND)) {
            for (int x = top_left.x + 1; x < screen.dim.width; x++) {
                if (!color_in_range(screen.pixels[y - 1][x], LIGHT_UND) && !color_in_range(screen.pixels[y - 1][x], DARK_UND)) {
                    return {static_cast<uint16_t>(x - top_left.x), static_cast<uint16_t>(y - top_left.y)};
                }
            }
        }
    }
    return {0, 0};
}

Game::Game(std::shared_ptr<Position> pos, std::shared_ptr<Dimension> board_dimensions, std::shared_ptr<Dimension> box_dimensions) {
    this->position = move(pos);
    this->board_dimensions = move(board_dimensions);
    this->box_dimensions = move(box_dimensions);

    int width = this->board_dimensions->width / this->box_dimensions->width;
    int height = this->board_dimensions->height / this->box_dimensions->height;

    this->board = std::make_shared<Board>(width, height);
    this->update();
}

std::shared_ptr<Board> Game::get_board() {
    return this->board;
}

Status Game::status() {
    Screenshot screen = screenshot(*this->position, *this->board_dimensions);

    for (int y = 0; y < this->board_dimensions->height; y++) {
        for (int x = 0; x < this->board_dimensions->width; x++) {
            Pixel pixel = screen.pixels[y][x];
            if (color_in_range(pixel, RESULTS)) {
                return LOST;
            }
        }
    }

    // Check if all tiles are discovered
   std::vector<Tile> undiscovered_tiles = this->board->get_undiscovered_tiles();
    if (undiscovered_tiles.empty()) {
        return WON;
    }

    return IN_PROGRESS;
}

void Game::click(int x, int y) {
    Position pos = box_mouse_position(x, y);
    move_mouse(pos);
    left_click();
}

void Game::flag(int x, int y) {
    Position pos = box_mouse_position(x, y);
    move_mouse(pos);
    right_click();
}

void Game::update() {
    move_mouse(Position(0, 0));
    Screenshot screen = screenshot(*this->position, *this->board_dimensions);

   std::vector<Tile> tiles = this->board->get_undiscovered_tiles();
    for (Tile tile : tiles) {
        int value = this->tile_value(tile.x, tile.y, screen);
        this->board->set_tile(tile.x, tile.y, value);
    }
}

int Game::tile_value(int x, int y, const Screenshot& screen) {
    Position mid = box_mouse_position(x, y);
    int mid_x = mid.x - this->position->x;
    int mid_y = mid.y - this->position->y;
    return classify_pixel(screen.pixels[mid_y][mid_x]);
}

Position Game::box_mouse_position(int x, int y) {
    uint16_t top_right_x = this->position->x + (this->box_dimensions->width * x);
    uint16_t top_right_y = this->position->y + (this->box_dimensions->height * y);
    return Position(top_right_x + this->box_dimensions->width / 2, top_right_y + this->box_dimensions->height / 2);
}

std::pair<Position, Dimension> Game::tile_range(int x, int y) {
    uint16_t left_x = position->x + (box_dimensions->width * x);
    uint16_t top_y = position->y + (box_dimensions->height * y);

    uint16_t right_x = left_x + box_dimensions->width - 1;
    uint16_t bottom_y = top_y + box_dimensions->height - 1;

    return std::make_pair(Position(left_x, top_y), Dimension(right_x - left_x + 1, bottom_y - top_y + 1));
}

std::unique_ptr<Game> find_game() {
    Screenshot screen = screenshot();

    // Find the top-left corner of the game board
    for (uint16_t y = 0; y < screen.dim.height; y++) {
        for (uint16_t x = 0; x < screen.dim.width; x++) {
            Pixel pixel = screen.pixels[y][x];
            if (color_in_range(pixel, LIGHT_UND)) {
                Position position = Position(x, y);
                Dimension box_dimensions = find_box_dimensions(screen, position);
                Dimension board_dimensions = find_board_dimensions(screen, position);

                if (box_dimensions.width > 0 && box_dimensions.height > 0 && board_dimensions.width > 0 && board_dimensions.height > 0) {
                    return std::make_unique<Game>(std::make_shared<Position>(position),
                        std::make_shared<Dimension>(board_dimensions), std::make_shared<Dimension>(box_dimensions));
                }
            }
        }
    }

    return nullptr;
}
