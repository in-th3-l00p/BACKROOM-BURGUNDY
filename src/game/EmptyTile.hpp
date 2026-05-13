#pragma once

#include "Tile.hpp"

namespace escape::game {
    class EmptyTile final : public Tile {
    public:
        EmptyTile();
        explicit EmptyTile(app::Color floor_tint);

        auto clone() const -> std::unique_ptr<Tile> override;

        auto is_passable() const noexcept -> bool override { return true; }
        auto cell_id() const noexcept -> int override { return 0; }
        auto kind() const -> std::string override { return "EmptyTile"; }
        auto base_color() const -> app::Color override { return floor_tint_; }
        auto sample(float wall_u, float wall_v, int side) const -> app::Color override;

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        app::Color floor_tint_;
    };
}
