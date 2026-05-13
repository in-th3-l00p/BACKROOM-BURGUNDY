#pragma once

#include "Texture.hpp"
#include "Tile.hpp"

#include <memory>

namespace escape::game {
    class TexturedWallTile final : public Tile {
    public:
        TexturedWallTile(int cell_id, std::shared_ptr<Texture> texture);

        auto clone() const -> std::unique_ptr<Tile> override;

        auto is_passable() const noexcept -> bool override { return false; }
        auto cell_id() const noexcept -> int override { return cell_id_; }
        auto kind() const -> std::string override { return "TexturedWallTile"; }
        auto base_color() const -> app::Color override;
        auto sample(float wall_u, float wall_v, int side) const -> app::Color override;

        auto texture() const noexcept -> const Texture& { return *texture_; }

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        int cell_id_;
        std::shared_ptr<Texture> texture_;
    };
}
