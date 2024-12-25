#include "screen.h"

Screen::Screen() {
    pos = Position(0, 0);
    dim = Dimension(
        static_cast<uint32_t>(GetSystemMetrics(SM_CXSCREEN)),
        static_cast<uint32_t>(GetSystemMetrics(SM_CYSCREEN))
    );
    stride = 0;  // Will be calculated in take_screenshot
}

Screen::Screen(Position p, Dimension d) : pos(p), dim(d), stride(0) {}

void Screen::take_screenshot() {
    if (dim.width == 0 || dim.height == 0) {
        throw ScreenshotException("Invalid dimensions: width and height must be greater than 0");
    }

    GdiResources resources;

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
    bi.biHeight = -static_cast<int32_t>(dim.height);  // Negative for top-down bitmap
    bi.biPlanes = 1;
    bi.biBitCount = 24;  // 24-bit RGB
    bi.biCompression = BI_RGB;

    // Calculate stride (bytes per row, padded to 4-byte boundary)
    stride = ((dim.width * 3 + 3) & ~3);

    // Allocate bitmap data
    bitmap_data = std::make_unique<uint8_t[]>(stride * dim.height);

    // Get bitmap data
    if (!GetDIBits(resources.memoryDC, resources.bitmap, 0, dim.height,
        bitmap_data.get(), reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS)) {
        throw ScreenshotException("Failed to get bitmap data");
    }
}

Pixel Screen::get_pixel(uint32_t x, uint32_t y) const {
    if (x >= dim.width || y >= dim.height || !bitmap_data) {
        return Pixel();  // Return black pixel for invalid coordinates
    }

    // Calculate offset into bitmap data
    size_t offset = y * stride + x * 3;
    return Pixel(
        bitmap_data[offset + 2],  // R
        bitmap_data[offset + 1],  // G
        bitmap_data[offset]       // B
    );
}

// Screen pixel iterator
Screen::PixelIterator::PixelIterator(const Screen* s, Position p) : screen(s), pos(p) {}

Pixel Screen::PixelIterator::pixel() const { 
    return screen->get_pixel(pos.x, pos.y);
}

Position Screen::PixelIterator::position() const { 
    return pos;
}

Screen::PixelIterator& Screen::PixelIterator::next() {
    if (++pos.x >= screen->get_dimension().width) {
        pos.x = 0;
        ++pos.y;
    }
    return *this;
}

bool Screen::PixelIterator::is_end() {
    return pos.x >= screen->get_dimension().width || pos.y >= screen->get_dimension().height;
}

// Skip to specific position
Screen::PixelIterator& Screen::PixelIterator::jump_to(Position new_pos) {
    pos = new_pos;
    return *this;
}

// Shift over 1 row
Screen::PixelIterator& Screen::PixelIterator::next_row() {
    ++pos.y;
    return *this;
}

Screen::PixelIterator Screen::begin() const { 
    return PixelIterator(this, Position(0, 0));
}

Screen::PixelIterator Screen::iterate_from(Position start_pos) const {
    return PixelIterator(this, start_pos);
}

void move_mouse(Position pos) {
    SetCursorPos(pos.x, pos.y);
}

void mouse_click(MouseAction action) {
    // Press the left mouse button
    INPUT inputDown = {};
    inputDown.type = INPUT_MOUSE;
    inputDown.mi.dwFlags = action == LEFT_CLICK ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;

    // Release the left mouse button
    INPUT inputUp = {};
    inputUp.type = INPUT_MOUSE;
    inputUp.mi.dwFlags = action == LEFT_CLICK ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;

    // Send both events (press and release)
    INPUT inputs[] = { inputDown, inputUp };
    SendInput(2, inputs, sizeof(INPUT));
}