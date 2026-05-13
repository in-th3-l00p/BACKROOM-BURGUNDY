#pragma once

#include "Window.hpp"

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <vector>

namespace escape::app {
    class Framebuffer {
    public:
        Framebuffer(int width, int height);

        auto width() const noexcept -> int { return width_; }
        auto height() const noexcept -> int { return height_; }
        auto pixel_count() const noexcept -> std::size_t { return pixels_.size() / 4; }

        auto pixels() noexcept -> std::uint8_t* { return pixels_.data(); }
        auto pixels() const noexcept -> const std::uint8_t* { return pixels_.data(); }
        auto pitch_bytes() const noexcept -> int { return width_ * 4; }

        void clear(Color color);
        void set_pixel(int x, int y, Color color) noexcept;
        void draw_vertical_strip(int column, int top_y, int bottom_y, Color color);

    private:
        int width_;
        int height_;
        std::vector<std::uint8_t> pixels_;
    };

    auto operator<<(std::ostream& stream, const Framebuffer& framebuffer) -> std::ostream&;
}
