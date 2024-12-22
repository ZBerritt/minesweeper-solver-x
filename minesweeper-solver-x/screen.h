#pragma once
#include <vector>
#include <cstdint>

struct Position {
    uint16_t x;
    uint16_t y;
    Position(uint16_t xx, uint16_t yy) : x(xx), y(yy) {}
};

struct Dimension {
    uint16_t width;
    uint16_t height;
    Dimension(uint16_t w, uint16_t h) : width(w), height(h) {}
};

struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    Pixel() : red(0), green(0), blue(0) {}
    Pixel(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}
};

struct Screenshot {
    Position pos;
    Dimension dim;
    std::vector<std::vector<Pixel>> pixels;
    Screenshot(Position p, Dimension d,std::vector<std::vector<Pixel>> px) : pos(p), dim(d), pixels(px) {}
};

// Board Colors
#define LIGHT_UND Pixel(170, 215, 81)
#define DARK_UND Pixel(162, 209, 73)
#define LIGHT_EMPTY Pixel(224, 195, 163)
#define DARK_EMPTY Pixel(211, 185, 157)
#define BORDER Pixel(126, 164, 53)
#define FLAG Pixel(242, 54, 7)
#define RESULTS Pixel(77, 193, 249)
#define NUM_ONE Pixel(25, 118, 210)
#define NUM_TWO Pixel(55, 141, 59)
#define NUM_THREE Pixel(211, 47, 47)
#define NUM_FOUR Pixel(123, 31, 162)
#define NUM_FIVE Pixel(255, 139, 0)
#define NUM_SIX Pixel(30, 157, 169)

// Screen utils

// Take a screenshot of a specific part of the screen
Screenshot screenshot(const Position& pos, const Dimension& dim);

// Take a screenshot of the entire screen
Screenshot screenshot();

// Move the mouse to this position
void move_mouse(Position pos);

void left_click();

void right_click();
