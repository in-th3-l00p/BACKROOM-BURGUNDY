#include "Map.hpp"

#include "../errors/Exceptions.hpp"
#include "EmptyTile.hpp"
#include "SolidWallTile.hpp"
#include "Texture.hpp"
#include "TexturedWallTile.hpp"

#include <fstream>
#include <memory>
#include <ostream>
#include <utility>

namespace escape::game {
    namespace {
        auto get_brick_texture() -> std::shared_ptr<Texture> {
            static auto texture = std::make_shared<Texture>(Texture::make_brick(
                64, 64,
                app::Color {.r = 170, .g = 70, .b = 60, .a = 255},
                app::Color {.r = 50,  .g = 30, .b = 20, .a = 255}));
            return texture;
        }

        auto get_checker_texture() -> std::shared_ptr<Texture> {
            static auto texture = std::make_shared<Texture>(Texture::make_checker(
                64, 64,
                app::Color {.r = 210, .g = 210, .b = 210, .a = 255},
                app::Color {.r = 70,  .g = 70,  .b = 70,  .a = 255},
                8));
            return texture;
        }

        auto get_grid_texture() -> std::shared_ptr<Texture> {
            static auto texture = std::make_shared<Texture>(Texture::make_grid(
                64, 64,
                app::Color {.r = 60,  .g = 90,  .b = 150, .a = 255},
                app::Color {.r = 200, .g = 230, .b = 255, .a = 255}));
            return texture;
        }
    }

    Map::Map(int width, int height, std::vector<std::unique_ptr<Tile>> tiles)
        : width_(width), height_(height), tiles_(std::move(tiles)) {
        if (width_ <= 0 || height_ <= 0) {
            throw errors::InvalidConfigError {"Map dimensions must be positive"};
        }
        if (tiles_.size() != static_cast<std::size_t>(width_ * height_)) {
            throw errors::MapLoadError {"Map tile count does not match width*height"};
        }
        for (const auto& tile : tiles_) {
            if (tile == nullptr) {
                throw errors::MapLoadError {"Map contains a null tile"};
            }
        }
    }

    Map::Map(int width, int height)
        : width_(width), height_(height), tiles_() {
        if (width_ <= 0 || height_ <= 0) {
            throw errors::InvalidConfigError {"Map dimensions must be positive"};
        }
        tiles_.reserve(static_cast<std::size_t>(width_ * height_));
        for (int i = 0; i < width_ * height_; ++i) {
            tiles_.push_back(std::make_unique<EmptyTile>());
        }
    }

    Map::Map(const Map& other)
        : width_(other.width_), height_(other.height_) {
        tiles_.reserve(other.tiles_.size());
        for (const auto& tile : other.tiles_) {
            tiles_.push_back(tile->clone());
        }
    }

    Map::Map(Map&& other) noexcept
        : width_(other.width_),
          height_(other.height_),
          tiles_(std::move(other.tiles_)) {
        other.width_ = 0;
        other.height_ = 0;
    }

    auto Map::operator=(Map other) noexcept -> Map& {
        swap(*this, other);
        return *this;
    }

    void swap(Map& left, Map& right) noexcept {
        using std::swap;
        swap(left.width_, right.width_);
        swap(left.height_, right.height_);
        swap(left.tiles_, right.tiles_);
    }

    auto Map::index_of(int x, int y) const -> std::size_t {
        return static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) + static_cast<std::size_t>(x);
    }

    auto Map::in_bounds(int x, int y) const noexcept -> bool {
        return x >= 0 && y >= 0 && x < width_ && y < height_;
    }

    auto Map::tile_at(int x, int y) const -> const Tile& {
        if (!in_bounds(x, y)) {
            throw errors::OutOfBoundsError {"Tile lookup out of bounds"};
        }
        return *tiles_[index_of(x, y)];
    }

    void Map::set_tile(int x, int y, std::unique_ptr<Tile> tile) {
        if (!in_bounds(x, y)) {
            throw errors::OutOfBoundsError {"Tile assignment out of bounds"};
        }
        if (tile == nullptr) {
            throw errors::InvalidConfigError {"Cannot set a null tile"};
        }
        tiles_[index_of(x, y)] = std::move(tile);
    }

    auto Map::cell_at(int x, int y) const -> int {
        if (!in_bounds(x, y)) {
            return 1;
        }
        return tiles_[index_of(x, y)]->cell_id();
    }

    auto Map::is_solid_at(int x, int y) const -> bool {
        if (!in_bounds(x, y)) {
            return true;
        }
        return !tiles_[index_of(x, y)]->is_passable();
    }

    auto Map::solid_tile_count() const -> std::size_t {
        std::size_t count = 0;
        for (const auto& tile : tiles_) {
            if (!tile->is_passable()) {
                ++count;
            }
        }
        return count;
    }

    auto Map::textured_tile_count() const -> std::size_t {
        std::size_t count = 0;
        for (const auto& tile : tiles_) {
            if (dynamic_cast<const TexturedWallTile*>(tile.get()) != nullptr) {
                ++count;
            }
        }
        return count;
    }

    auto Map::demo_tile_for_cell(int cell_value) -> std::unique_ptr<Tile> {
        switch (cell_value) {
        case 0:
            return std::make_unique<EmptyTile>();
        case 1:
            return std::make_unique<TexturedWallTile>(1, get_brick_texture());
        case 2:
            return std::make_unique<SolidWallTile>(2, app::Color {.r = 50, .g = 200, .b = 90, .a = 255});
        case 3:
            return std::make_unique<TexturedWallTile>(3, get_grid_texture());
        case 4:
            return std::make_unique<TexturedWallTile>(4, get_checker_texture());
        case 5:
            return std::make_unique<SolidWallTile>(5, app::Color {.r = 240, .g = 200, .b = 70, .a = 255});
        default:
            return std::make_unique<SolidWallTile>(cell_value,
                app::Color {.r = 200, .g = 200, .b = 200, .a = 255});
        }
    }

    auto Map::load_from_file(const std::string& path) -> Map {
        auto file = std::ifstream {path};
        if (!file.is_open()) {
            throw errors::MapLoadError {"Failed to open map file: " + path};
        }

        int width = 0;
        int height = 0;
        if (!(file >> width >> height)) {
            throw errors::MapLoadError {"Map header must contain width and height"};
        }

        auto tiles = std::vector<std::unique_ptr<Tile>> {};
        tiles.reserve(static_cast<std::size_t>(width * height));
        for (int i = 0; i < width * height; ++i) {
            int value = 0;
            if (!(file >> value)) {
                throw errors::MapLoadError {"Map data is shorter than declared dimensions"};
            }
            tiles.push_back(demo_tile_for_cell(value));
        }
        return Map {width, height, std::move(tiles)};
    }

    auto Map::build_demo_map() -> Map {
        constexpr int width = 24;
        constexpr int height = 24;
        const int raw[height][width] = {
            {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,4},
            {4,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,2,0,5,0,2,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,4},
            {4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,2,2,0,2,2,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,3,3,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,3,3,0,3,3,4},
            {4,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
            {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
        };

        auto tiles = std::vector<std::unique_ptr<Tile>> {};
        tiles.reserve(static_cast<std::size_t>(width * height));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                tiles.push_back(demo_tile_for_cell(raw[y][x]));
            }
        }
        return Map {width, height, std::move(tiles)};
    }

    auto operator<<(std::ostream& stream, const Map& map) -> std::ostream& {
        stream << "Map{width=" << map.width()
               << ", height=" << map.height()
               << ", tiles=" << map.tile_count()
               << ", solid_tiles=" << map.solid_tile_count()
               << ", textured_tiles=" << map.textured_tile_count() << "}";
        return stream;
    }
}
