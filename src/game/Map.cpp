#include "Map.hpp"

#include <fstream>
#include <ostream>
#include <sstream>
#include <stdexcept>

namespace escape::game {
    Map::Map(int width, int height, std::vector<int> cells)
        : width_(width), height_(height), cells_(std::move(cells)) {
        if (width_ <= 0 || height_ <= 0) {
            throw std::invalid_argument {"Map dimensions must be positive"};
        }
        if (cells_.size() != static_cast<std::size_t>(width_ * height_)) {
            throw std::invalid_argument {"Map cell count does not match width*height"};
        }
    }

    Map::Map(int width, int height)
        : Map(width, height, std::vector<int>(static_cast<std::size_t>(width * height), 0)) {}

    auto Map::in_bounds(int x, int y) const noexcept -> bool {
        return x >= 0 && y >= 0 && x < width_ && y < height_;
    }

    auto Map::cell_at(int x, int y) const -> int {
        if (!in_bounds(x, y)) {
            return 1;
        }
        return cells_[static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) + static_cast<std::size_t>(x)];
    }

    void Map::set_cell(int x, int y, int value) {
        if (!in_bounds(x, y)) {
            throw std::out_of_range {"Map cell coordinates out of range"};
        }
        cells_[static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) + static_cast<std::size_t>(x)] = value;
    }

    auto Map::is_solid_at(int x, int y) const -> bool {
        return cell_at(x, y) > 0;
    }

    auto Map::load_from_file(const std::string& path) -> Map {
        auto file = std::ifstream {path};
        if (!file.is_open()) {
            throw std::runtime_error {"Failed to open map file: " + path};
        }

        int width = 0;
        int height = 0;
        if (!(file >> width >> height)) {
            throw std::runtime_error {"Map header must contain width and height"};
        }

        auto cells = std::vector<int> {};
        cells.reserve(static_cast<std::size_t>(width * height));
        for (int i = 0; i < width * height; ++i) {
            int value = 0;
            if (!(file >> value)) {
                throw std::runtime_error {"Map data is shorter than declared dimensions"};
            }
            cells.push_back(value);
        }

        return Map {width, height, std::move(cells)};
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

        auto cells = std::vector<int> {};
        cells.reserve(static_cast<std::size_t>(width * height));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                cells.push_back(raw[y][x]);
            }
        }
        return Map {width, height, std::move(cells)};
    }

    auto operator<<(std::ostream& stream, const Map& map) -> std::ostream& {
        stream << "Map{width=" << map.width()
               << ", height=" << map.height()
               << ", cells=" << map.cell_count() << "}";
        return stream;
    }
}
