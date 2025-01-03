#include <iostream>
#include <algorithm>
#include "utils/util.h"

#include "board.h"

Board::Board(int w, int h) {
    height = h;
    width = w;
    tiles.resize(height * width);

    // Add coordinates
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            tiles[to_index(j, i)] = Tile(j, i);
        }
    }
}

const Tile& Board::get_tile(int x, int y) const {
    return tiles[to_index(x, y)];
}

void Board::set_tile(int x, int y, int val) {
    tiles[to_index(x, y)].value = val;
}

std::vector<Tile> Board::get_undiscovered_tiles() {
   std::vector<Tile> undiscovered;
   undiscovered.reserve(height * width);

   for (const auto& tile : tiles) {
        if (tile.value == UNDISCOVERED) {
            undiscovered.push_back(tile);
        }
   }
   undiscovered.shrink_to_fit();
   return undiscovered;
}

std::vector<Tile> Board::get_surrounding_tiles(Tile t) const {
    return surrounding_tiles<Tile>(t.x, t.y, width, height, [&](int xx, int yy) {
        return tiles[to_index(xx, yy)];
        });
}

std::vector<Tile> Board::get_border_tiles() {
    std::vector<Tile> border;
    for (Tile tile : tiles) {
        if (tile.value != UNDISCOVERED) {
            std::vector<Tile> surrounding = get_surrounding_tiles(tile);
            for (Tile s : surrounding) {
                if (s.value == UNDISCOVERED) {
                    border.push_back(tile);
                    break;
                }
            }
        }
    }
    return border;
}

int Board::remaining_nearby_mines(Tile t) const {
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
    return std::count_if(tiles.begin(), tiles.end(),
        [](const Tile& tile) { return tile.value != UNDISCOVERED; });
}
