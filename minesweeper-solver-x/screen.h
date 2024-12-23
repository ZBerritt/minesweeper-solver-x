#pragma once
#include <vector>
#include <cstdint>

struct Position {
    uint32_t x;
    uint32_t y;
    Position(uint32_t xx = 0, uint32_t yy = 0) : x(xx), y(yy) {} // Added default constructor
};

struct Dimension {
    uint32_t width;
    uint32_t height;
    Dimension(uint32_t w = 0, uint32_t h = 0) : width(w), height(h) {} // Added default constructor
};

struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    Pixel() : red(0), green(0), blue(0) {}
    Pixel(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}
};

class Screenshot {
public:
    Screenshot();
    Screenshot(Position p, Dimension d); // Removed const from parameters
    void take();
    Position get_position() const;  // Removed unnecessary const
    Dimension get_dimension() const; // Removed unnecessary const
    Pixel get_pixel(uint32_t x, uint32_t y) const; // Added const correctness
private:
    Position pos;  // Removed const
    Dimension dim; // Removed const
    std::vector<Pixel> pixels;
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

void move_mouse(Position pos);

void left_click();

void right_click();
