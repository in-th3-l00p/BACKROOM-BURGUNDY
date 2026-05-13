#pragma once

#include "Tile.hpp"

namespace escape::game {
    class EmissiveWallTile final : public Tile {
    public:
        EmissiveWallTile(int cell_id, app::Color glow_color, float pulse_strength = 0.25F);

        auto clone() const -> std::unique_ptr<Tile> override;

        auto is_passable() const noexcept -> bool override { return false; }
        auto cell_id() const noexcept -> int override { return cell_id_; }
        auto kind() const -> std::string override { return "EmissiveWallTile"; }
        auto base_color() const -> app::Color override { return glow_color_; }
        auto sample(float wall_u, float wall_v, int side) const -> app::Color override;

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        int cell_id_;
        app::Color glow_color_;
        float pulse_strength_;
    };
}
