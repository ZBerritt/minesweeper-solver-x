#include "screen.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Helper function to capture a screenshot of a specified screen area
Screenshot screenshot(const Position& pos, const Dimension& dim) {
    // Get specs of the screenshot
    uint16_t x = pos.x;
    uint16_t y = pos.y;
    uint16_t width = dim.width;
    uint16_t height = dim.height;
       
    // Hook windows API
    HDC hScreenDC = GetDC(nullptr);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hBitmap));
    BitBlt(hMemoryDC, x, y, width, height, hScreenDC, 0, 0, SRCCOPY);
    hBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hOldBitmap));


    DeleteDC(hMemoryDC);
    DeleteDC(hScreenDC);
    return Screenshot(pos, dim, pixels);
}

// Helper function to capture the entire screen
Screenshot screenshot() {
    // Get the screen dimensions
    uint16_t screenWidth = static_cast<uint16_t>(GetSystemMetrics(SM_CXSCREEN));
    uint16_t screenHeight = static_cast<uint16_t>(GetSystemMetrics(SM_CYSCREEN));

    Position pos = {0, 0};
    Dimension dim = {screenWidth, screenHeight};

    return screenshot(pos, dim);
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
    INPUT inputs[] = {inputDown, inputUp};
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
    INPUT inputs[] = {inputDown, inputUp};
    SendInput(2, inputs, sizeof(INPUT));
}
