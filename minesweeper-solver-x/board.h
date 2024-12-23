#pragma once

#include <vector>

constexpr int UNDISCOVERED = -1;
constexpr int MINE = -2;
constexpr int UNKNOWN = -3;

struct Tile {
    int x;
    int y;
    int value;
    Tile() : x(0), y(0), value(UNDISCOVERED) {}
    Tile(int x, int y, int value = UNDISCOVERED) 
        : x(x), y(y), value(value) {}
};

class Board {
    public:
        Board(int w, int h);
        Tile get_tile(int x, int y);
        void set_tile(int x, int y, int val);
        std::vector<Tile> get_all_tiles();
        std::vector<Tile> get_undiscovered_tiles();
        std::vector<Tile> get_remaining_tiles();
        std::vector<Tile> get_surrounding_tiles(int x, int y);
        std::vector<Tile> get_border_tiles();
        std::vector<Tile> get_undiscovered_borders();
        void print();

    private:
        int height;
        int width;
        std::vector<std::vector<Tile>> boxes;
};
