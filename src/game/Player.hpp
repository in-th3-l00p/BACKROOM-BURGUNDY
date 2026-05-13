#pragma once

#include "Vector2.hpp"

#include <iosfwd>

namespace escape::game {
    class Map;

    class Player {
    public:
        Player(Vector2 position, Vector2 direction, Vector2 camera_plane,
               float move_speed, float rotation_speed);

        Player(const Player& other);
        auto operator=(const Player& other) -> Player&;
        ~Player();

        auto position() const noexcept -> const Vector2& { return position_; }
        auto direction() const noexcept -> const Vector2& { return direction_; }
        auto camera_plane() const noexcept -> const Vector2& { return camera_plane_; }
        auto move_speed() const noexcept -> float { return move_speed_; }
        auto rotation_speed() const noexcept -> float { return rotation_speed_; }

        void try_move_forward(float delta_time_seconds, const Map& map);
        void try_move_backward(float delta_time_seconds, const Map& map);
        void try_strafe_left(float delta_time_seconds, const Map& map);
        void try_strafe_right(float delta_time_seconds, const Map& map);
        void rotate_left(float delta_time_seconds);
        void rotate_right(float delta_time_seconds);

    private:
        void try_translate(float delta_x, float delta_y, const Map& map);
        void rotate(float angle_radians);

        Vector2 position_;
        Vector2 direction_;
        Vector2 camera_plane_;
        float move_speed_;
        float rotation_speed_;
    };

    auto operator<<(std::ostream& stream, const Player& player) -> std::ostream&;
}
