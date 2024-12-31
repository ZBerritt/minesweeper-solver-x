#include <chrono>
#include "board.h"

enum Status {
    IN_PROGRESS,
    LOST,
    WON
};

// Abstract class representing a Minesweeper game
class Game {
public:
    virtual Status status();
    virtual void update();
    virtual void click(int x, int y);
    virtual void flag(int x, int y);
    std::shared_ptr<Board> get_board() const { return board; }
protected:
    Game(std::string n, int w, int h, std::chrono::duration<std::chrono::milliseconds> md) : name(n), width(w), height(h),
        move_delay(md), board(std::make_shared<Board>(w, h)) {}
private:
    std::string name;
    int width;
    int height;
    std::chrono::duration<std::chrono::milliseconds> move_delay;
    std::shared_ptr<Board> board;
};