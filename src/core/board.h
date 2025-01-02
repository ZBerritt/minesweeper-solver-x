#pragma once

#include <vector>

constexpr int MINE = -1;
constexpr int UNDISCOVERED = -2;
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
        const Tile& get_tile(int x, int y) const;
        void set_tile(int x, int y, int val);
        int get_height() const { return height; }
		int get_width() const { return width; }
        const std::vector<Tile>& get_all_tiles() const { return tiles; }
        std::vector<Tile> get_undiscovered_tiles();
        std::vector<Tile> get_surrounding_tiles(Tile t) const;
        std::vector<Tile> get_border_tiles();
		int remaining_nearby_mines(Tile t);
        int discovered_count();
        void print();

    private:
        int height;
        int width;
        std::vector<Tile> tiles;

        inline int to_index(int x, int y) const { return y * width + x;  }
};
