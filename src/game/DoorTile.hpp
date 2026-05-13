#pragma once

#include "Texture.hpp"
#include "Tile.hpp"

#include <memory>

namespace escape::game {
    class DoorTile final : public Tile {
    public:
        enum class State { Closed, Opening, Open, Closing };

        DoorTile(int cell_id, std::shared_ptr<Texture> texture);

        auto clone() const -> std::unique_ptr<Tile> override;

        auto is_passable() const noexcept -> bool override;
        auto cell_id() const noexcept -> int override { return cell_id_; }
        auto kind() const -> std::string override { return "DoorTile"; }
        auto base_color() const -> app::Color override;
        auto sample(float wall_u, float wall_v, int side) const -> app::Color override;
        auto passes_ray_through() const noexcept -> bool override;

        auto state() const noexcept -> State { return state_; }
        auto open_fraction() const noexcept -> float { return open_fraction_; }

        void toggle() noexcept;
        void advance(float delta_time_seconds) noexcept;

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        int cell_id_;
        std::shared_ptr<Texture> texture_;
        State state_;
        float open_fraction_;
        float open_speed_;
    };
}
