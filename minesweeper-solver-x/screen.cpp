#include "screen.h"

// Resource caching

bool Screen::init_resources() {
    if (screen_dc) return true; // Already initialized

    screen_dc = GetDC(nullptr);
    if (!screen_dc) return false;

    memory_dc = CreateCompatibleDC(screen_dc);
    if (!memory_dc) {
        ReleaseDC(nullptr, screen_dc);
        screen_dc = nullptr;
        return false;
    }

    if (!bitmap) {
		stride = ((dim.width * 3 + 3) & ~3);  // Calculate stride (bytes per row, padded to 4-byte boundary)
        bitmap = CreateCompatibleBitmap(screen_dc, dim.width, dim.height);

        bitmap_data = std::make_unique<unsigned char[]>(stride * dim.height);
    }

    return true;
}

void Screen::clean_resources() {
    if (bitmap) {
        DeleteObject(bitmap);
        bitmap = nullptr;
    }
    if (memory_dc) {
        DeleteDC(memory_dc);
        memory_dc = nullptr;
    }
    if (screen_dc) {
        ReleaseDC(nullptr, screen_dc);
        screen_dc = nullptr;
    }
}

Screen::Screen(Position p, Dimension d) : pos(p), dim(d), stride(0),
    screen_dc(nullptr), memory_dc(nullptr), bitmap(nullptr) {
	init_resources();
}

Screen::~Screen() {
	clean_resources();
}

void Screen::take_screenshot() {
    if (dim.width == 0 || dim.height == 0) {
        throw ScreenshotException("Invalid dimensions: width and height must be greater than 0");
    }

    if (!screen_dc || !memory_dc || !bitmap) {
        throw ScreenshotException("Resources were not properly initialized");
    }

    HBITMAP oldBitmap = static_cast<HBITMAP>(SelectObject(memory_dc, bitmap));

    // Copy screen to bitmap
    if (!BitBlt(memory_dc, 0, 0, dim.width, dim.height,
        screen_dc, pos.x, pos.y, SRCCOPY)) {
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

    // Get bitmap data
    if (!GetDIBits(memory_dc, bitmap, 0, dim.height,
        bitmap_data.get(), reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS)) {
        SelectObject(memory_dc, oldBitmap);
        throw ScreenshotException("Failed to get bitmap data");
    }
}

Pixel Screen::get_pixel(unsigned int x, unsigned int y) const {
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