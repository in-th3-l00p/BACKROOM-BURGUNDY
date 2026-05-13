#include "DoorTile.hpp"

#include "../errors/Exceptions.hpp"

#include <algorithm>
#include <ostream>

namespace escape::game {
    DoorTile::DoorTile(int cell_id, std::shared_ptr<Texture> texture)
        : cell_id_(cell_id),
          texture_(std::move(texture)),
          state_(State::Closed),
          open_fraction_(0.0F),
          open_speed_(1.5F) {
        if (texture_ == nullptr) {
            throw errors::InvalidConfigError {"DoorTile requires a non-null texture"};
        }
    }

    auto DoorTile::clone() const -> std::unique_ptr<Tile> {
        return std::make_unique<DoorTile>(*this);
    }

    auto DoorTile::is_passable() const noexcept -> bool {
        return state_ == State::Open;
    }

    auto DoorTile::passes_ray_through() const noexcept -> bool {
        return open_fraction_ > 0.0F;
    }

    auto DoorTile::base_color() const -> app::Color {
        return texture_->sample(0, 0);
    }

    auto DoorTile::sample(float wall_u, float wall_v, int side) const -> app::Color {
        if (wall_u < open_fraction_) {
            return app::Color {.r = 0, .g = 0, .b = 0, .a = 0};
        }
        const auto shifted_u = (wall_u - open_fraction_) / std::max(1.0F - open_fraction_, 0.0001F);
        return shade_for_side(texture_->sample_normalized(shifted_u, wall_v), side);
    }

    void DoorTile::toggle() noexcept {
        switch (state_) {
        case State::Closed:
        case State::Closing:
            state_ = State::Opening;
            break;
        case State::Open:
        case State::Opening:
            state_ = State::Closing;
            break;
        }
    }

    void DoorTile::advance(float delta_time_seconds) noexcept {
        const auto step = open_speed_ * delta_time_seconds;
        switch (state_) {
        case State::Opening:
            open_fraction_ = std::min(1.0F, open_fraction_ + step);
            if (open_fraction_ >= 1.0F) {
                state_ = State::Open;
            }
            break;
        case State::Closing:
            open_fraction_ = std::max(0.0F, open_fraction_ - step);
            if (open_fraction_ <= 0.0F) {
                state_ = State::Closed;
            }
            break;
        case State::Open:
        case State::Closed:
            break;
        }
    }

    void DoorTile::do_print(std::ostream& stream) const {
        const char* state_name = "Closed";
        switch (state_) {
        case State::Closed: state_name = "Closed"; break;
        case State::Opening: state_name = "Opening"; break;
        case State::Open: state_name = "Open"; break;
        case State::Closing: state_name = "Closing"; break;
        }
        stream << "DoorTile{id=" << cell_id_
               << ", state=" << state_name
               << ", open=" << open_fraction_ << "}";
    }
}
