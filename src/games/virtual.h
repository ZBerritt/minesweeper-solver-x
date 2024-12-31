#pragma once
#include <memory>
#include <vector>

#include "core/game.h"

struct VirtualTile {
	bool mine;
	bool clicked;
	VirtualTile() : mine(false), clicked(false) {}
};

class Virtual : public Game {
public:
	Virtual(int w, int h, int m);
	void click(int x, int y) override;
	void flag(int x, int y) override;
	void update() override;
	Status status() override;
	std::shared_ptr<Board> get_board();
private:
	int mines;
	std::vector<std::vector<VirtualTile>> tiles;
	std::shared_ptr<Board> board;
	std::vector<std::pair<int, int>> get_surrounding_tiles(int x, int y);
	void create_board(int start_x, int start_y);
	int tile_value(int x, int y);
};