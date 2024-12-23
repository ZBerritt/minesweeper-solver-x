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

static Dimension find_box_dimensions(Screenshot& screen, const Position& top_left) {
    for (int y = top_left.y + 1; y < screen.get_dimension().height; y++) {
        Pixel pixel = screen.get_pixel(top_left.x, y);
        if (color_in_range(pixel, DARK_UND)) {
            for (int x = top_left.x + 1; x < screen.get_dimension().width; x++) {
                pixel = screen.get_pixel(x, y - 1);
                if (color_in_range(pixel, DARK_UND)) {
                    return {static_cast<uint32_t>(x - top_left.x + 2),
                        static_cast<uint32_t>(y - top_left.y + 2)};
                }
            }
        }
    }
    return {0, 0};
}

static Dimension find_board_dimensions(Screenshot& screen, const Position& top_left) {
    for (int y = top_left.y + 1; y < screen.get_dimension().height; y++) {
        Pixel pixel = screen.get_pixel(top_left.x, y);
        if (!color_in_range(pixel, LIGHT_UND) && !color_in_range(pixel, DARK_UND)) {
            for (int x = top_left.x + 1; x < screen.get_dimension().width; x++) {
                pixel = screen.get_pixel(x, y - 1);
                if (!color_in_range(pixel, LIGHT_UND) && !color_in_range(pixel, DARK_UND)) {
                    return {static_cast<uint32_t>(x - top_left.x), static_cast<uint32_t>(y - top_left.y)};
                }
            }
        }
    }
    return {0, 0};
}

Game::Game(const Position& pos, const Dimension& board_dimensions, const Dimension& box_dimensions)
    : position(pos), board_dimensions(board_dimensions), box_dimensions(box_dimensions), screen(position, board_dimensions) {

    int width = board_dimensions.width / box_dimensions.width;
    int height = board_dimensions.height / box_dimensions.height;

    board = std::make_shared<Board>(width, height);
    update();
}

std::shared_ptr<Board> Game::get_board() {
    return board;
}

Status Game::status() {
    screen.take();

    for (int y = 0; y < board_dimensions.height; y++) {
        for (int x = 0; x < board_dimensions.width; x++) {
            Pixel pixel = screen.get_pixel(x, y);
            if (color_in_range(pixel, RESULTS)) {
                return LOST;
            }
        }
    }

    // Check if all tiles are discovered
   std::vector<Tile> undiscovered_tiles = board->get_undiscovered_tiles();
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
    screen.take();

   std::vector<Tile> tiles = board->get_undiscovered_tiles();
    for (Tile tile : tiles) {
        int value = tile_value(tile.x, tile.y, screen);
        board->set_tile(tile.x, tile.y, value);
    }
}

int Game::tile_value(int x, int y, Screenshot& screen) {
    Position mid = box_mouse_position(x, y);
    int mid_x = mid.x - position.x;
    int mid_y = mid.y - position.y;
    return classify_pixel(screen.get_pixel(mid_x, mid_y));
}

Position Game::box_mouse_position(int x, int y) {
    uint32_t top_right_x = position.x + (box_dimensions.width * x);
    uint32_t top_right_y = position.y + (box_dimensions.height * y);
    return Position(top_right_x + box_dimensions.width / 2, top_right_y + box_dimensions.height / 2);
}

std::pair<Position, Dimension> Game::tile_range(int x, int y) {
    uint32_t left_x = position.x + (box_dimensions.width * x);
    uint32_t top_y = position.y + (box_dimensions.height * y);

    uint32_t right_x = left_x + box_dimensions.width - 1;
    uint32_t bottom_y = top_y + box_dimensions.height - 1;

    return std::make_pair(Position(left_x, top_y), Dimension(right_x - left_x + 1, bottom_y - top_y + 1));
}

std::unique_ptr<Game> find_game() {
    Screenshot screen = Screenshot();

    // Find the top-left corner of the game board
    for (uint32_t y = 0; y < screen.get_dimension().height; y++) {
        for (uint32_t x = 0; x < screen.get_dimension().width; x++) {
            Pixel pixel = screen.get_pixel(x, y);
            if (color_in_range(pixel, LIGHT_UND)) {
                Position position = Position(x, y);
                Dimension box_dimensions = find_box_dimensions(screen, position);
                Dimension board_dimensions = find_board_dimensions(screen, position);

                if (box_dimensions.width > 0 && box_dimensions.height > 0 && board_dimensions.width > 0 && board_dimensions.height > 0) {
                    return std::make_unique<Game>(position, board_dimensions, box_dimensions);
                }
            }
        }
    }

    return nullptr;
}
