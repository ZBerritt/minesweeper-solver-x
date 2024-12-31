#include <cmath>
#include <chrono>
#include <thread>
#include <iostream>

#include "google.h"

// Generates our 81 sample points for tile detection
std::vector<Pixel> Google::generate_sample_points(int x, int y) const {
    const auto [range_pos, range_dim] = tile_range(x, y);

    std::vector<Pixel> samples;
    samples.reserve(81);
    for (float rx = 0.1f; rx < 1.0f; rx += 0.1f) {
        for (float ry = 0.1f; ry < 1.0f; ry += 0.1f) {
            uint32_t px = range_pos.x + static_cast<uint32_t>(rx * range_dim.width);
            uint32_t py = range_pos.y + static_cast<uint32_t>(ry * range_dim.height);
            samples.push_back(screen.get_pixel(px, py));
        }
    }

    return samples;
}

static inline bool color_in_range(const Pixel& a, const Pixel& b, int range = 10) {
    return abs(a.red - b.red) <= range &&
        abs(a.green - b.green) <= range &&
        abs(a.blue - b.blue) <= range;
}

static double get_color_distance(const Pixel& a, const Pixel& b) {
    int dr = static_cast<int>(a.red) - b.red;
    int dg = static_cast<int>(a.green) - b.green;
    int db = static_cast<int>(a.blue) - b.blue;
    return std::sqrt(dr * dr + dg * dg + db * db);
};

Google::Google(const Position& pos, const Dimension& board_dim, const Dimension& box_dim) : 
    Game("Google", board_dim.width / box_dim.width, board_dim.height / box_dim.height, std::chrono::milliseconds(250))
    , position(pos)
    , board_dimensions(board_dim)
    , box_dimensions(box_dim)
    , screen(pos, board_dim)
{
    update();
}

// Assumes the board's screenshot has already been taken
Status Google::status() {
    // Check win condition
    std::vector<Tile> undiscovered = board->get_undiscovered_tiles();
    if (undiscovered.empty()) {
        return WON;
    }

    // Check for game over condition
    for (Screen::PixelIterator it = screen.begin(); !it.is_end(); it.next()) {
        if (color_in_range(it.pixel(), RESULTS)) {
            return LOST;
        }
    }
    return IN_PROGRESS;
}

void Google::click(int x, int y) {
    const Position pos = box_mouse_position(x, y);
    move_mouse(pos);
    mouse_click(LEFT_CLICK);
}

void Google::flag(int x, int y) {
    const Position pos = box_mouse_position(x, y);
    move_mouse(pos);
    mouse_click(RIGHT_CLICK);
}

void Google::update() {
    move_mouse({ 0, 0 }); // Move mouse out of the way of the game board
    screen.take_screenshot();

    std::vector<Tile> tiles = board->get_all_tiles();
    for (const Tile& tile : tiles) {
        if (tile.value < MINE) { // Don't update already detected values
        int value = tile_value(tile.x, tile.y);
        board->set_tile(tile.x, tile.y, value);
        }
    }
}


// TODO: Detection on small tiles (i.e. hard difficulty) is not completely working
int Google::tile_value(int x, int y) const {
	// Populate sample points
    std::vector<Pixel> samples = generate_sample_points(x, y);

    // Base Case - Is the tile undiscovered?
    int undiscovered_matches = 0;
    for (const auto& pixel : samples) {
		if (classify_pixel(pixel) == UNDISCOVERED && ++undiscovered_matches >= 42) { // We require over half of the samples to be undiscovered
			return UNDISCOVERED;
        }
    }

	// Advamced Color Matching
	std::vector<int> value_votes(TILE_VALUE_COUNT, 0);

    for (const auto& pixel : samples) {
        // Enhanced color distance calculation
        double min_distance = 999999.0;
        int best_match = 0;

        for (const auto& [color, value] : pixel_classification) {
            if (value <= 0) continue;
            double distance = get_color_distance(pixel, color);
            if (distance < min_distance) {
                min_distance = distance;
                best_match = value;
            }
        }

        if (min_distance < 30.0f) {  // Adjusted threshold
            value_votes[best_match]++;
        }
    }

    // Find the value with the most votes
    int max_votes = 0;
    int detected_value = 0;
    for (int i = 0; i < value_votes.size(); i++) {
        if (value_votes[i] > max_votes) {
            max_votes = value_votes[i];
            detected_value = i;
        }
    }

    // Return detected value if exists
    if (max_votes > 0) {
        return detected_value;
    }

    // Final Case - The tile may be empty
    int empty_matches = 0;
    for (const auto& pixel : samples) {
        if (classify_pixel(pixel) == 0 && ++empty_matches >= 64) { // We need more confidence to label something as 0
            return 0;
        }
    }
	return UNKNOWN;
}

// Relative to the computer screen
Position Google::box_mouse_position(int x, int y) const {
    return Position(
        position.x + (box_dimensions.width * x) + box_dimensions.width / 2,
        position.y + (box_dimensions.height * y) + box_dimensions.height / 2
    );
}


// Relative to the screen object
std::pair<Position, Dimension> Google::tile_range(int x, int y) const {
    const uint32_t left_x = box_dimensions.width * x;
    const uint32_t top_y = box_dimensions.height * y;
    const uint32_t right_x = left_x + box_dimensions.width - 1;
    const uint32_t bottom_y = top_y + box_dimensions.height - 1;

    return std::make_pair(Position(left_x, top_y), Dimension(right_x - left_x + 1, bottom_y - top_y + 1));
}

int Google::classify_pixel(const Pixel& pixel) {
    // Iterate through the mapping to find a matching color range
    for (const auto& [range, value] : pixel_classification) {
        if (color_in_range(pixel, range)) {
            return value;
        }
    }

    // Return UNKNOWN if no match is found
    return UNKNOWN;
}

// Finding the game board

static Dimension find_box_dimensions(Screen& screen, const Position& top_left) {
    uint32_t width = 0;
    uint32_t height = 0;

    // Find width
    
    for (Screen::PixelIterator it = screen.iterate_from(top_left); !it.is_end(); it.next()) {
        Position pos = it.position();
        if (color_in_range(it.pixel(), DARK_UND, 5)) {
            width = pos.x - top_left.x;
            break;
        }
    }

    // Find y
    for (Screen::PixelIterator it = screen.iterate_from(top_left); !it.is_end(); it.next_row()) {
        Position pos = it.position();
        if (color_in_range(it.pixel(), DARK_UND, 5)) {
            height = pos.y - top_left.y;
            break;
        }
    }
    
    // Return dimensions
    return Dimension(width, height);
}

static Dimension find_board_dimensions(Screen& screen, const Position& top_left) {
    uint32_t width = 0;
    uint32_t height = 0;

    // Find width
    for (Screen::PixelIterator it = screen.iterate_from(top_left); !it.is_end(); it.next()) {
        Position pos = it.position();
        Pixel pixel = it.pixel();
        if (!color_in_range(pixel, LIGHT_UND, 5) && !color_in_range(pixel, DARK_UND, 5)) {
            width = pos.x - top_left.x;
            break;
        }
    }

    // Find y
    for (Screen::PixelIterator it = screen.iterate_from(top_left); !it.is_end(); it.next_row()) {
        Position pos = it.position();
        Pixel pixel = it.pixel();
        if (!color_in_range(pixel, LIGHT_UND, 5) && !color_in_range(pixel, DARK_UND, 5)) {
            height = pos.y - top_left.y;
            break;
        }
    }

    // Return dimensions
    return Dimension(width, height);
}

std::unique_ptr<Google> Google::find_game() {
    std::cout << "Searching for Google board, please make sure its on the screen" << std::endl;
    Screen screen;
   
    while (true) {
        screen.take_screenshot();
        for (Screen::PixelIterator it = screen.begin(); !it.is_end(); it.next()) {
            if (color_in_range(it.pixel(), LIGHT_UND)) {
                Position position = it.position();
                Dimension box_dimensions = find_box_dimensions(screen, position);
                Dimension board_dimensions = find_board_dimensions(screen, position);

                if (box_dimensions.width > 10 && box_dimensions.height > 10 &&
                    board_dimensions.width > 0 && board_dimensions.height > 0) {
                    return std::make_unique<Google>(position, board_dimensions, box_dimensions);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    
    return nullptr;
}