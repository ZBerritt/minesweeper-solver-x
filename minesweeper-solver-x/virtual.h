#include <memory>
#include <vector>

#include "board.h"

enum VirtualStatus {
	V_IN_PROGRESS,
	V_WON,
	V_LOST
};

struct VirtualTile {
	bool mine;
	bool clicked;
	VirtualTile() : mine(false), clicked(false) {}
};

class Virtual {
public:
	Virtual(int w, int h, int m);
	void click(int x, int y);
	void update();
	VirtualStatus status();
	std::shared_ptr<Board> get_board();
private:
	int mines;
	std::vector<std::vector<VirtualTile>> tiles;
	std::shared_ptr<Board> board;
	std::vector<std::pair<int, int>> get_surrounding_tiles(int x, int y);
	void create_board(int start_x, int start_y);
	int tile_value(int x, int y);
};