#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "screen.h"

Screenshot::Screenshot() : Screenshot(
    Position(0, 0),
    Dimension(
        static_cast<uint32_t>(GetSystemMetrics(SM_CXSCREEN)),
        static_cast<uint32_t>(GetSystemMetrics(SM_CYSCREEN))
    )
) {
}

Screenshot::Screenshot(Position p, Dimension d) : pos(p), dim(d) {
    take();
}

void Screenshot::take() {
    if (dim.width == 0 || dim.height == 0) {
        return; // Prevent invalid dimensions
    }

    // Extract constants
    uint32_t x = pos.x;
    uint32_t y = pos.y;
    uint32_t width = dim.width;
    uint32_t height = dim.height;

    // Clear and reserve space for the screenshot
    pixels.clear();
    pixels.reserve(width * height);

    // Create device contexts and bitmap
    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) return;

    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    if (!hMemoryDC) {
        ReleaseDC(nullptr, hScreenDC);
        return;
    }

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    if (!hBitmap) {
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return;
    }

    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hBitmap));

    // Copy screen to bitmap
    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, x, y, SRCCOPY);

    // Prepare bitmap info
    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -static_cast<int32_t>(height); // Negative for top-down bitmap
    bi.biPlanes = 1;
    bi.biBitCount = 24; // 24 bits per pixel (RGB)
    bi.biCompression = BI_RGB;

    // Calculate proper row stride (must be DWORD-aligned)
    int stride = ((width * 3 + 3) & ~3);
    std::vector<uint8_t> bitmapData(stride * height);

    // Extract bitmap data
    if (GetDIBits(hMemoryDC, hBitmap, 0, height, bitmapData.data(),
        reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS)) {

        // Convert bitmap data to pixel format (R, G, B)
        pixels.clear();
        pixels.reserve(width * height);

        for (uint32_t y = 0; y < height; ++y) {
            for (uint32_t x = 0; x < width; ++x) {
                size_t offset = y * stride + x * 3;
                pixels.emplace_back(
                    bitmapData[offset + 2],  // R
                    bitmapData[offset + 1],  // G
                    bitmapData[offset]       // B
                );
            }
        }
    }

    // Cleanup GDI objects
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(nullptr, hScreenDC);
}

Position Screenshot::get_position() const {
    return pos;
}

Dimension Screenshot::get_dimension() const {
    return dim;
}

Pixel Screenshot::get_pixel(uint32_t x, uint32_t y) const {
    if (x >= dim.width || y >= dim.height || pixels.empty()) {
        return Pixel(); // Return black pixel for invalid coordinates
    }
    return pixels[(y * dim.width) + x];
}

// Move the mouse to a specific position on the screen
void move_mouse(Position pos) {
    SetCursorPos(pos.x, pos.y);
}

void left_click() {
    // Press the left mouse button
    INPUT inputDown = {};
    inputDown.type = INPUT_MOUSE;
    inputDown.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    // Release the left mouse button
    INPUT inputUp = {};
    inputUp.type = INPUT_MOUSE;
    inputUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;

    // Send both events (press and release)
    INPUT inputs[] = { inputDown, inputUp };
    SendInput(2, inputs, sizeof(INPUT));
}

// Simulate a right mouse click
void right_click() {
    // Press the right mouse button
    INPUT inputDown = {};
    inputDown.type = INPUT_MOUSE;
    inputDown.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

    // Release the right mouse button
    INPUT inputUp = {};
    inputUp.type = INPUT_MOUSE;
    inputUp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;

    // Send both events (press and release)
    INPUT inputs[] = { inputDown, inputUp };
    SendInput(2, inputs, sizeof(INPUT));
}