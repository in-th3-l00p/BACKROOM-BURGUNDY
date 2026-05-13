#include "Framebuffer.hpp"

#include "../errors/Exceptions.hpp"

#include <algorithm>
#include <ostream>

namespace escape::app {
    Framebuffer::Framebuffer(int width, int height)
        : width_(width), height_(height),
          pixels_(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4, 0) {
        if (width_ <= 0 || height_ <= 0) {
            throw errors::InvalidConfigError {"Framebuffer dimensions must be positive"};
        }
    }

    void Framebuffer::clear(Color color) {
        for (std::size_t i = 0; i < pixels_.size(); i += 4) {
            pixels_[i + 0] = color.r;
            pixels_[i + 1] = color.g;
            pixels_[i + 2] = color.b;
            pixels_[i + 3] = color.a;
        }
    }

    void Framebuffer::set_pixel(int x, int y, Color color) noexcept {
        if (x < 0 || y < 0 || x >= width_ || y >= height_) {
            return;
        }
        const std::size_t index = (static_cast<std::size_t>(y) * static_cast<std::size_t>(width_)
                                 + static_cast<std::size_t>(x)) * 4;
        pixels_[index + 0] = color.r;
        pixels_[index + 1] = color.g;
        pixels_[index + 2] = color.b;
        pixels_[index + 3] = color.a;
    }

    void Framebuffer::draw_vertical_strip(int column, int top_y, int bottom_y, Color color) {
        const int start = std::max(0, std::min(top_y, bottom_y));
        const int end = std::min(height_ - 1, std::max(top_y, bottom_y));
        for (int y = start; y <= end; ++y) {
            set_pixel(column, y, color);
        }
    }

    auto operator<<(std::ostream& stream, const Framebuffer& framebuffer) -> std::ostream& {
        stream << "Framebuffer{width=" << framebuffer.width()
               << ", height=" << framebuffer.height()
               << ", pixels=" << framebuffer.pixel_count() << "}";
        return stream;
    }
}
