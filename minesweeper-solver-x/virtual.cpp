#include <random>
#include <algorithm>
#include "virtual.h"

Virtual::Virtual(int w, int h, int m) {
	mines = m;
	board = std::make_shared<Board>(w, h);
}

void Virtual::create_board(int start_x, int start_y) {
	// Ban the starting tile and its surrounding tiles from being mines
	std::vector<std::pair<int, int>> illegal_tiles = get_surrounding_tiles(start_x, start_y);
	illegal_tiles.push_back(std::pair<int, int>(start_x, start_y));

	// Determine possible mine coordinates
	std::vector<Tile> board_tiles = board->get_all_tiles();
	std::vector<std::pair<int, int>> possible_mine_coordinates;
	for (Tile tile : board_tiles) {
		for (std::pair<int, int> illegal_tile : illegal_tiles) {
			if (tile.x == illegal_tile.first && tile.y == illegal_tile.second) {
				continue;
			}
			possible_mine_coordinates.push_back(std::pair<int, int>(tile.x, tile.y));
		}
	}

	// Get mine coordinates
	std::random_device rd;
	std::mt19937 generator(rd());
	std::vector<std::pair<int, int>> mine_coordinates;
	std::sample(possible_mine_coordinates.begin(), possible_mine_coordinates.end(), 
		std::back_inserter(mine_coordinates), mines, generator);

	// Create the internal board
	tiles = std::vector<std::vector<VirtualTile>>(board->get_height(), std::vector<VirtualTile>(board->get_width()));
	for (std::pair<int, int> mine_coordinate : mine_coordinates) {
		tiles[mine_coordinate.second][mine_coordinate.first].mine = true;
	}
}

std::vector<std::pair<int, int>> Virtual::get_surrounding_tiles(int x, int y) {
	std::vector<std::pair<int, int>> surrounding_tiles;
	for (int i = y - 1; i <= y + 1; i++) {
		for (int j = x - 1; j <= x + 1; j++) {
			if (i >= 0 && i < board->get_height() && j >= 0 && j < board->get_width()) {
				surrounding_tiles.push_back(std::pair<int, int>(j, i));
			}
		}
	}
	return surrounding_tiles;
}

int Virtual::tile_value(int x, int y) {
	if (tiles[y][x].mine) {
		return MINE;
	}

	int value = 0;
	std::vector<std::pair<int, int>> surrounding_tiles = get_surrounding_tiles(x, y);
	for (std::pair<int, int> surrounding_tile : surrounding_tiles) {
		if (tiles[surrounding_tile.second][surrounding_tile.first].mine) {
			value++;
		}
	}
	return value;
}

void Virtual::click(int x, int y) {
	if (tiles.empty()) {
		create_board(x, y);
	}
	
	if (tiles[y][x].clicked) {
		return;
	}
	VirtualTile* current_vtile = &tiles[y][x];
	current_vtile->clicked = true;

	int value = tile_value(x, y);
	if (value == 0 && !current_vtile->mine) {
		std::vector<std::pair<int, int>> surrounding_tiles = get_surrounding_tiles(x, y);
		for (std::pair<int, int> surrounding_tile : surrounding_tiles) {
			click(surrounding_tile.first, surrounding_tile.second);
		}
	}
}

void Virtual::update() {
	for (int i = 0; i < board->get_height(); i++) {
		for (int j = 0; j < board->get_width(); j++) {
			VirtualTile* vtile = &tiles[i][j];
			if (vtile->clicked) {
				board->set_tile(j, i, tile_value(j, i));
			}
		}
	}
}

VirtualStatus Virtual::status() {
	if (tiles.empty()) {
		return V_IN_PROGRESS;
	}

	for (int i = 0; i < board->get_height(); i++) {
		for (int j = 0; j < board->get_width(); j++) {
			VirtualTile vtile = tiles[i][j];
			if (vtile.clicked && vtile.mine) {
				return V_LOST;
			}
			else if (!vtile.clicked && !vtile.mine) {
				return V_IN_PROGRESS;
			}
		}
	}
	
	return V_WON;
}

std::shared_ptr<Board> Virtual::get_board() {
	return board;
}