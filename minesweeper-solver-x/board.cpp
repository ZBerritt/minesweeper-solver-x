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

void Board::print() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Tile tile = boxes[i][j];
            if (tile.value == UNDISCOVERED) {
                std::cout << "-";
            } else if (tile.value == MINE) {
                std::cout << "F";
            } else {
                std::cout << boxes[i][j].value;
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}
