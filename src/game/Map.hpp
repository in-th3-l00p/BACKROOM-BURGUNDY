#pragma once

#include "Tile.hpp"

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace escape::game {
    class Map {
    public:
        Map(int width, int height, std::vector<std::unique_ptr<Tile>> tiles);
        Map(int width, int height);

        Map(const Map& other);
        Map(Map&& other) noexcept;
        auto operator=(Map other) noexcept -> Map&;
        ~Map() = default;

        friend void swap(Map& left, Map& right) noexcept;

        auto width() const noexcept -> int { return width_; }
        auto height() const noexcept -> int { return height_; }
        auto tile_count() const noexcept -> std::size_t { return tiles_.size(); }

        auto tile_at(int x, int y) const -> const Tile&;
        void set_tile(int x, int y, std::unique_ptr<Tile> tile);

        auto cell_at(int x, int y) const -> int;
        auto is_solid_at(int x, int y) const -> bool;
        auto in_bounds(int x, int y) const noexcept -> bool;

        auto solid_tile_count() const -> std::size_t;
        auto textured_tile_count() const -> std::size_t;

        static auto load_from_file(const std::string& path) -> Map;
        static auto build_demo_map() -> Map;

    private:
        static auto demo_tile_for_cell(int cell_value) -> std::unique_ptr<Tile>;

        auto index_of(int x, int y) const -> std::size_t;

        int width_;
        int height_;
        std::vector<std::unique_ptr<Tile>> tiles_;
    };

    auto operator<<(std::ostream& stream, const Map& map) -> std::ostream&;
}
