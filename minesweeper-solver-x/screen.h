#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define STRICT
#include <windows.h>
#include <memory>
#include <stdexcept>

class ScreenshotException : public std::runtime_error {
public:
    explicit ScreenshotException(const char* message) : std::runtime_error(message) {}
};

struct Position {
    unsigned int x;
    unsigned int y;
    Position(unsigned int x = 0, unsigned int y = 0) : x(x), y(y) {}
};

struct Dimension {
    unsigned int width;
    unsigned int height;
    Dimension(unsigned int w = 0, unsigned int h = 0) : width(w), height(h) {}
};

struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    Pixel() : red(0), green(0), blue(0) {}
    Pixel(unsigned char r, unsigned char g, unsigned char b) : red(r), green(g), blue(b) {}
};

class Screen {
public:
    // Iterator
    struct PixelIterator {
        const Screen* screen;
        Position pos;

        PixelIterator(const Screen* s, Position p);

        Pixel pixel() const;
        Position position() const;

        PixelIterator& next();
        PixelIterator& jump_to(Position new_pos);
        PixelIterator& next_row();
        bool is_end();
    };
    Screen(Position p = {0, 0}, 
        Dimension d = { static_cast<unsigned int>(GetSystemMetrics(SM_CXSCREEN)), static_cast<unsigned int>(GetSystemMetrics(SM_CYSCREEN)) });
    ~Screen();
    void take_screenshot();
    Position get_position() const { return pos; }
    Dimension get_dimension() const { return dim; }
    Pixel get_pixel(unsigned int x, unsigned int y) const;

    // Iteration
    PixelIterator begin() const;
    PixelIterator iterate_from(Position start_pos) const;

    // Screenshot resources and methods
    HDC screen_dc;
    HDC memory_dc;
    HBITMAP bitmap;

private:
    Position pos;
    Dimension dim;
    unsigned int stride;  // Added for proper pixel addressing
    std::unique_ptr<unsigned char[]> bitmap_data;  // Raw bitmap data instead of vector of Pixels
       
    bool init_resources();
    void clean_resources();
};

// Screen movement and actions

enum MouseAction {
    LEFT_CLICK,
    RIGHT_CLICK
};

// Screen utils
void move_mouse(Position pos);
void mouse_click(MouseAction action);