#include "games/google.h"
#include "games/virtual.h"

#include "game.h"


std::shared_ptr<Game> Game::get_game(std::string type, const std::chrono::milliseconds& delay_override) {
    if (type == "google") {
        return Google::find_game();
    }
    else if (type == "veasy") {
        return std::make_unique<Virtual>(10, 10, 10, delay_override);
    }
    else if (type == "vmedium") {
        return std::make_unique<Virtual>(15, 15, 40, delay_override);
    }
    else if (type == "vhard") {
        return std::make_unique<Virtual>(20, 20, 100, delay_override);
    }
    else if (type == "vimpossible") {
       return std::make_unique<Virtual>(50, 50, 1000, delay_override);
    }
    else {
        return nullptr;
    }
}