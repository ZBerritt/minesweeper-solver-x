// screen.h
#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define STRICT
#include <windows.h>
#include <vector>
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
    Screen();
    Screen(Position p, Dimension d);
    void take_screenshot();
    Position get_position() const { return pos;  };
    Dimension get_dimension() const { return dim; };
    const Pixel& get_pixel(uint32_t x, uint32_t y) const;

private:
    Position pos;
    Dimension dim;
    std::vector<Pixel> pixels;

    // RAII wrapper for GDI resources
    struct GdiResources {
        HDC screenDC;
        HDC memoryDC;
        HBITMAP bitmap;
        HBITMAP oldBitmap;

        GdiResources() : screenDC(nullptr), memoryDC(nullptr),
            bitmap(nullptr), oldBitmap(nullptr) {
        }

        ~GdiResources() {
            if (oldBitmap) SelectObject(memoryDC, oldBitmap);
            if (bitmap) DeleteObject(bitmap);
            if (memoryDC) DeleteDC(memoryDC);
            if (screenDC) ReleaseDC(nullptr, screenDC);
        }
    };
};


// Screen utils

void move_mouse(Position pos);

void left_click();

void right_click();
