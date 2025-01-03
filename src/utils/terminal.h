#pragma once
#include <memory>
#include <string>
#include "core/board.h"


class BoardDisplay {
private:
	std::shared_ptr<Board> board;
	size_t board_height;
	size_t terminal_height;
	std::vector<std::string> output_buffer;
	const size_t max_output_lines = 25;

	void clear_screen();
	void move_cursor(int row, int col);
	std::string board_to_string() const;

public:
	BoardDisplay(std::shared_ptr<Board> b);
	~BoardDisplay();
	void update_board();
	void print(const std::string& message);
};