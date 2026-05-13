#pragma once

#include "../app/Window.hpp"

#include <iosfwd>
#include <memory>
#include <string>

namespace escape::game {
    class Tile {
    public:
        Tile() = default;
        Tile(const Tile&) = default;
        Tile(Tile&&) noexcept = default;
        auto operator=(const Tile&) -> Tile& = default;
        auto operator=(Tile&&) noexcept -> Tile& = default;
        virtual ~Tile() = default;

        virtual auto clone() const -> std::unique_ptr<Tile> = 0;

        virtual auto is_passable() const noexcept -> bool = 0;
        virtual auto cell_id() const noexcept -> int = 0;
        virtual auto kind() const -> std::string = 0;
        virtual auto base_color() const -> app::Color = 0;
        virtual auto sample(float wall_u, float wall_v, int side) const -> app::Color = 0;

        virtual auto passes_ray_through() const noexcept -> bool { return false; }

        void print(std::ostream& stream) const;

    protected:
        virtual void do_print(std::ostream& stream) const = 0;

        static auto shade_for_side(app::Color color, int side) -> app::Color;
    };

    auto operator<<(std::ostream& stream, const Tile& tile) -> std::ostream&;
}
