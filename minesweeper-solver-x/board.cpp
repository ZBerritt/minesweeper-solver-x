#include <iostream>
#include <algorithm>

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

std::vector<Tile> Board::get_surrounding_tiles(Tile t) {
    std::vector<Tile> surrounding;
    surrounding.reserve(8);

    const int start_i = std::max(0, t.y - 1);
    const int end_i = std::min(height - 1, t.y + 1);
    const int start_j = std::max(0, t.x - 1);
    const int end_j = std::min(width - 1, t.x + 1);

    for (int i = start_i; i <= end_i; i++) {
        for (int j = start_j; j <= end_j; j++) {
            surrounding.push_back(tiles[to_index(j, i)]);
        }
    }
    return surrounding;
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
    return std::count_if(tiles.begin(), tiles.end(),
        [](const Tile& tile) { return tile.value != UNDISCOVERED; });
}

void Board::print() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Tile tile = tiles[to_index(j, i)];
            if (tile.value == UNDISCOVERED) {
                std::cout << "-";
            }
            else if (tile.value == MINE) {
                std::cout << "F";
            }
            else if (tile.value == UNKNOWN) {
                std::cout << "?";
            } else {
                std::cout << tile.value;
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}
