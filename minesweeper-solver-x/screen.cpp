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
        throw ScreenshotException("Invalid dimensions: width and height must be greater than 0");
    }

    GdiResources resources;

    // Initialize GDI resources
    resources.screenDC = GetDC(nullptr);
    if (!resources.screenDC) {
        throw ScreenshotException("Failed to get screen DC");
    }

    resources.memoryDC = CreateCompatibleDC(resources.screenDC);
    if (!resources.memoryDC) {
        throw ScreenshotException("Failed to create memory DC");
    }

    resources.bitmap = CreateCompatibleBitmap(resources.screenDC, dim.width, dim.height);
    if (!resources.bitmap) {
        throw ScreenshotException("Failed to create compatible bitmap");
    }

    resources.oldBitmap = static_cast<HBITMAP>(SelectObject(resources.memoryDC, resources.bitmap));

    // Copy screen to bitmap
    if (!BitBlt(resources.memoryDC, 0, 0, dim.width, dim.height,
        resources.screenDC, pos.x, pos.y, SRCCOPY)) {
        throw ScreenshotException("Failed to copy screen content");
    }

    // Prepare bitmap info
    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = dim.width;
    bi.biHeight = -static_cast<int32_t>(dim.height); // Negative for top-down bitmap
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;

    // Calculate stride and allocate bitmap data
    int stride = ((dim.width * 3 + 3) & ~3);
    std::vector<uint8_t> bitmapData(stride * dim.height);

    // Get bitmap data
    if (!GetDIBits(resources.memoryDC, resources.bitmap, 0, dim.height,
        bitmapData.data(), reinterpret_cast<BITMAPINFO*>(&bi),
        DIB_RGB_COLORS)) {
        throw ScreenshotException("Failed to get bitmap data");
    }

    // Process bitmap data
    pixels.clear();
    pixels.reserve(dim.width * dim.height);

    for (uint32_t y = 0; y < dim.height; ++y) {
        for (uint32_t x = 0; x < dim.width; ++x) {
            size_t offset = y * stride + x * 3;
            pixels.emplace_back(
                bitmapData[offset + 2],  // R
                bitmapData[offset + 1],  // G
                bitmapData[offset]       // B
            );
        }
    }
}

const Pixel& Screenshot::get_pixel(uint32_t x, uint32_t y) const {
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