
class Game {
public:
    Game(int height, int width);
    std::shared_ptr<Board> get_board() const { return board; }
    Status status();
    void update();
    void click(int x, int y);
    void flag(int x, int y);
};