#include "terminal.h"
#include <iostream>
#include <sstream>
#include <iomanip>


void BoardDisplay::clear_screen() {
	std::cout << "\033[2J\033[1;1H";
}

void BoardDisplay::move_cursor(int row, int col) {
	std::cout << "\033[" << row << ";" << col << "H";
}

std::string BoardDisplay::board_to_string() const {
    std::ostringstream oss;
    oss << "Minesweeper Solver X:\n";
    const std::vector<Tile>& tiles = board->get_all_tiles();

    // Add column numbers
    oss << "   "; // Padding for row numbers
    for (int x = 0; x < board->get_width(); x++) {
        oss << std::setw(2) << x << " ";
    }
    oss << "\n";

    // Add separator line
    oss << "   " << std::string(board->get_width() * 3, '-') << "\n";

    for (int y = 0; y < board->get_height(); y++) {
        oss << std::setw(2) << y << "|"; // Row numbers
        for (int x = 0; x < board->get_width(); x++) {
            const Tile& tile = tiles[y * board->get_width() + x];
            oss << " ";
            if (tile.value == UNDISCOVERED) {
                oss << "-";
            }
            else if (tile.value == MINE) {
                oss << "X";
            }
            else if (tile.value == UNKNOWN) {
                oss << "?";
            }
            else {
                oss << tile.value;
            }
            oss << " ";
        }
        oss << "\n";
    }
    return oss.str();
}

BoardDisplay::BoardDisplay(std::shared_ptr<Board> b) : board(b) {
    board_height = static_cast<size_t>(board->get_height()) + 3;
	terminal_height = board_height + max_output_lines + 3;
	clear_screen();

	move_cursor(board_height + 2, 1);
	std::cout << std::string(board->get_width(), '-') << std::endl;

	output_buffer.reserve(max_output_lines);
}

BoardDisplay::~BoardDisplay() {
    // Move cursor to the end of all output before destroying
    move_cursor(terminal_height + 1, 1);
    std::cout << std::flush;
}

void BoardDisplay::update_board() {
	move_cursor(1, 1);
	std::cout << board_to_string() << std::flush;
}

void BoardDisplay::print(const std::string& message) {
	output_buffer.push_back(message);
	if (output_buffer.size() > max_output_lines) {
		output_buffer.erase(output_buffer.begin());
	}

	move_cursor(board_height + 3, 1);
	std::cout << "\033[J";
	for (const std::string& line : output_buffer) {
		std::cout << line << "\n";
	}
	std::cout << std::flush;
}