#include "Texture.hpp"

#include "../errors/Exceptions.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <ostream>
#include <sstream>

namespace escape::game {
    std::size_t Texture::invalid_count_ = 0;

    Texture::Texture(int width, int height, std::vector<app::Color> pixels)
        : width_(width), height_(height), pixels_(std::move(pixels)) {
        if (width_ <= 0 || height_ <= 0) {
            ++invalid_count_;
            throw errors::InvalidConfigError {"Texture dimensions must be positive"};
        }
        if (pixels_.size() != static_cast<std::size_t>(width_ * height_)) {
            ++invalid_count_;
            throw errors::InvalidConfigError {"Texture pixel count does not match width*height"};
        }
    }

    auto Texture::sample(int x, int y) const -> app::Color {
        const int sx = std::clamp(x, 0, width_ - 1);
        const int sy = std::clamp(y, 0, height_ - 1);
        return pixels_[static_cast<std::size_t>(sy) * static_cast<std::size_t>(width_) + static_cast<std::size_t>(sx)];
    }

    auto Texture::sample_normalized(float u, float v) const -> app::Color {
        u = std::clamp(u, 0.0F, 1.0F);
        v = std::clamp(v, 0.0F, 1.0F);
        const int x = static_cast<int>(u * static_cast<float>(width_));
        const int y = static_cast<int>(v * static_cast<float>(height_));
        return sample(x, y);
    }

    auto Texture::make_checker(int width, int height, app::Color light, app::Color dark, int cell) -> Texture {
        if (cell <= 0) {
            cell = 1;
        }
        auto pixels = std::vector<app::Color> {};
        pixels.reserve(static_cast<std::size_t>(width * height));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const bool on = ((x / cell) + (y / cell)) % 2 == 0;
                pixels.push_back(on ? light : dark);
            }
        }
        return Texture {width, height, std::move(pixels)};
    }

    auto Texture::make_brick(int width, int height, app::Color brick, app::Color mortar) -> Texture {
        auto pixels = std::vector<app::Color> {};
        pixels.reserve(static_cast<std::size_t>(width * height));
        for (int y = 0; y < height; ++y) {
            const bool row_offset = (y / 16) % 2 == 0;
            for (int x = 0; x < width; ++x) {
                const int shifted_x = row_offset ? x : (x + 32) % width;
                const bool mortar_y = (y % 16) < 2;
                const bool mortar_x = (shifted_x % 64) < 2;
                pixels.push_back((mortar_y || mortar_x) ? mortar : brick);
            }
        }
        return Texture {width, height, std::move(pixels)};
    }

    auto Texture::make_gradient(int width, int height, app::Color top, app::Color bottom) -> Texture {
        auto pixels = std::vector<app::Color> {};
        pixels.reserve(static_cast<std::size_t>(width * height));
        for (int y = 0; y < height; ++y) {
            const float t = static_cast<float>(y) / static_cast<float>(std::max(height - 1, 1));
            const auto r = static_cast<unsigned char>(top.r + (bottom.r - top.r) * t);
            const auto g = static_cast<unsigned char>(top.g + (bottom.g - top.g) * t);
            const auto b = static_cast<unsigned char>(top.b + (bottom.b - top.b) * t);
            for (int x = 0; x < width; ++x) {
                pixels.push_back(app::Color {r, g, b, 255});
            }
        }
        return Texture {width, height, std::move(pixels)};
    }

    auto Texture::make_grid(int width, int height, app::Color base, app::Color line) -> Texture {
        auto pixels = std::vector<app::Color> {};
        pixels.reserve(static_cast<std::size_t>(width * height));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const bool is_line = (x % 16 == 0) || (y % 16 == 0);
                pixels.push_back(is_line ? line : base);
            }
        }
        return Texture {width, height, std::move(pixels)};
    }

    auto Texture::load_from_ppm(const std::string& path) -> Texture {
        auto file = std::ifstream {path, std::ios::binary};
        if (!file.is_open()) {
            ++invalid_count_;
            throw errors::TextureLoadError {"Failed to open texture file: " + path};
        }

        std::string magic;
        int width = 0;
        int height = 0;
        int max_value = 0;
        if (!(file >> magic >> width >> height >> max_value) || magic != "P6") {
            ++invalid_count_;
            throw errors::TextureLoadError {"Invalid PPM header in: " + path};
        }
        file.get();

        auto pixels = std::vector<app::Color> {};
        pixels.reserve(static_cast<std::size_t>(width * height));
        for (int i = 0; i < width * height; ++i) {
            unsigned char rgb[3] {};
            if (!file.read(reinterpret_cast<char*>(rgb), 3)) {
                ++invalid_count_;
                throw errors::TextureLoadError {"PPM pixel data shorter than declared: " + path};
            }
            pixels.push_back(app::Color {rgb[0], rgb[1], rgb[2], 255});
        }
        return Texture {width, height, std::move(pixels)};
    }

    auto Texture::invalid_texture_count() noexcept -> std::size_t {
        return invalid_count_;
    }

    auto operator<<(std::ostream& stream, const Texture& texture) -> std::ostream& {
        stream << "Texture{width=" << texture.width()
               << ", height=" << texture.height()
               << ", pixels=" << texture.pixel_count() << "}";
        return stream;
    }
}
