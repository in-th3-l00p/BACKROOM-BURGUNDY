#pragma once

#include "Tile.hpp"

namespace escape::game {
    class SolidWallTile final : public Tile {
    public:
        SolidWallTile(int cell_id, app::Color color);

        auto clone() const -> std::unique_ptr<Tile> override;

        auto is_passable() const noexcept -> bool override { return false; }
        auto cell_id() const noexcept -> int override { return cell_id_; }
        auto kind() const -> std::string override { return "SolidWallTile"; }
        auto base_color() const -> app::Color override { return color_; }
        auto sample(float wall_u, float wall_v, int side) const -> app::Color override;

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        int cell_id_;
        app::Color color_;
    };
}
