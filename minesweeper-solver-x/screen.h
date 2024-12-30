#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define STRICT
#include <windows.h>
#include <memory>
#include <stdexcept>
#include <cstdint>

class ScreenshotException : public std::runtime_error {
public:
    explicit ScreenshotException(const char* message) : std::runtime_error(message) {}
};

struct Position {
    uint32_t x;
    uint32_t y;
    Position(uint32_t x = 0, uint32_t y = 0) : x(x), y(y) {}
};

struct Dimension {
    uint32_t width;
    uint32_t height;
    Dimension(uint32_t w = 0, uint32_t h = 0) : width(w), height(h) {}
};

struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    Pixel() : red(0), green(0), blue(0) {}
    Pixel(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}
};

class Screen {
public:
    // Iterator
    struct PixelIterator {
        const Screen* screen;
        Position pos;

        PixelIterator(const Screen* s, Position p);

        Pixel pixel() const noexcept;
        Position position() const noexcept;

        PixelIterator& next() noexcept;
        PixelIterator& jump_to(Position new_pos) noexcept;
        PixelIterator& next_row() noexcept;
        bool is_end() noexcept;
    };
    Screen(Position p = {0, 0}, 
        Dimension d = { static_cast<uint32_t>(GetSystemMetrics(SM_CXVIRTUALSCREEN)), static_cast<uint32_t>(GetSystemMetrics(SM_CYVIRTUALSCREEN)) });
    ~Screen();
    void take_screenshot();
    Position get_position() const { return pos; }
    Dimension get_dimension() const { return dim; }
    Pixel get_pixel(uint32_t x, uint32_t y) const noexcept;

    // Iteration
    PixelIterator begin() const noexcept;
    PixelIterator iterate_from(Position start_pos) const noexcept;

    // Screenshot resources and methods
    HDC screen_dc;
    HDC memory_dc;
    HBITMAP bitmap;

private:
    Position pos;
    Dimension dim;
    uint32_t stride;  // Added for proper pixel addressing
    std::unique_ptr<uint8_t[]> bitmap_data;  // Raw bitmap data instead of vector of Pixels
       
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