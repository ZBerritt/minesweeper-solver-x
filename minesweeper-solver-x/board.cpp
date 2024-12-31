#include <iostream>

#include "board.h"

Board::Board(int w, int h) {
    height = h;
    width = w;
    boxes = std::vector<std::vector<Tile>>(height, std::vector<Tile>(width));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            boxes[i][j] = Tile(j, i);
        }
    }
}

Tile Board::get_tile(int x, int y) {
    return boxes[y][x];
}

void Board::set_tile(int x, int y, int val) {
    boxes[y][x].value = val;
}

std::vector<Tile> Board::get_all_tiles() {
   std::vector<Tile> all_tiles;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            all_tiles.push_back(boxes[i][j]);
        }
    }
    return all_tiles;
}

std::vector<Tile> Board::get_undiscovered_tiles() {
   std::vector<Tile> undiscovered;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (boxes[i][j].value == UNDISCOVERED) {
                undiscovered.push_back(boxes[i][j]);
            }
        }
    }
    return undiscovered;
}

std::vector<Tile> Board::get_surrounding_tiles(Tile t) {
    std::vector<Tile> surrounding;
    for (int i = t.y - 1; i <= t.y + 1; i++) {
        for (int j = t.x - 1; j <= t.x + 1; j++) {
            if (i >= 0 && i < height && j >= 0 && j < width) {
                surrounding.push_back(boxes[i][j]);
            }
        }
    }
    return surrounding;
}

std::vector<Tile> Board::get_border_tiles() {
    std::vector<Tile> border;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Tile t = boxes[i][j];
            if (t.value != UNDISCOVERED) {
                std::vector<Tile> surrounding = get_surrounding_tiles(t);
                for (Tile s : surrounding) {
                    if (s.value == UNDISCOVERED) {
                        border.push_back(t);
                        break;
                    }
                }
            }
        }
    }
    return border;
}

int Board::remaining_nearby_mines(Tile t) {
    int remaining_mines = t.value;
    std::vector<Tile> surrounding = get_surrounding_tiles(t);
    for (Tile s : surrounding) {
        if (s.value == MINE) {
            remaining_mines--;
        }
    }
    return remaining_mines;
}

int Board::discovered_count() {
	int count = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (boxes[i][j].value != UNDISCOVERED) {
				count++;
			}
		}
	}
	return count;
}

void Board::print() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Tile tile = boxes[i][j];
            if (tile.value == UNDISCOVERED) {
                std::cout << "-";
            }
            else if (tile.value == MINE) {
                std::cout << "F";
            }
            else if (tile.value == UNKNOWN) {
                std::cout << "?";
            } else {
                std::cout << boxes[i][j].value;
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}
