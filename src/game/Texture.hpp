#pragma once

#include "../app/Window.hpp"

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace escape::game {
    class Texture {
    public:
        Texture(int width, int height, std::vector<app::Color> pixels);

        auto width() const noexcept -> int { return width_; }
        auto height() const noexcept -> int { return height_; }
        auto pixel_count() const noexcept -> std::size_t { return pixels_.size(); }

        auto sample(int x, int y) const -> app::Color;
        auto sample_normalized(float u, float v) const -> app::Color;

        static auto make_checker(int width, int height, app::Color light, app::Color dark, int cell) -> Texture;
        static auto make_brick(int width, int height, app::Color brick, app::Color mortar) -> Texture;
        static auto make_gradient(int width, int height, app::Color top, app::Color bottom) -> Texture;
        static auto make_grid(int width, int height, app::Color base, app::Color line) -> Texture;

        static auto load_from_ppm(const std::string& path) -> Texture;

        static auto invalid_texture_count() noexcept -> std::size_t;

    private:
        int width_;
        int height_;
        std::vector<app::Color> pixels_;

        static std::size_t invalid_count_;
    };

    auto operator<<(std::ostream& stream, const Texture& texture) -> std::ostream&;
}
