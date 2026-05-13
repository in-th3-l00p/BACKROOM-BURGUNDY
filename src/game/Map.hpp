#pragma once

#include <iosfwd>
#include <string>
#include <vector>

namespace escape::game {
    class Map {
    public:
        Map(int width, int height, std::vector<int> cells);
        Map(int width, int height);

        auto width() const noexcept -> int { return width_; }
        auto height() const noexcept -> int { return height_; }

        auto cell_at(int x, int y) const -> int;
        void set_cell(int x, int y, int value);

        auto is_solid_at(int x, int y) const -> bool;
        auto in_bounds(int x, int y) const noexcept -> bool;

        auto cell_count() const noexcept -> std::size_t { return cells_.size(); }

        static auto load_from_file(const std::string& path) -> Map;
        static auto build_demo_map() -> Map;

    private:
        int width_;
        int height_;
        std::vector<int> cells_;
    };

    auto operator<<(std::ostream& stream, const Map& map) -> std::ostream&;
}
