#include "Player.hpp"

#include "Map.hpp"

#include <cmath>
#include <ostream>

namespace escape::game {
    Player::Player(Vector2 position, Vector2 direction, Vector2 camera_plane,
                   float move_speed, float rotation_speed)
        : position_(position),
          direction_(direction),
          camera_plane_(camera_plane),
          move_speed_(move_speed),
          rotation_speed_(rotation_speed) {}

    Player::Player(const Player& other) = default;

    auto Player::operator=(const Player& other) -> Player& {
        if (this == &other) {
            return *this;
        }
        position_ = other.position_;
        direction_ = other.direction_;
        camera_plane_ = other.camera_plane_;
        move_speed_ = other.move_speed_;
        rotation_speed_ = other.rotation_speed_;
        return *this;
    }

    Player::~Player() = default;

    void Player::try_translate(float delta_x, float delta_y, const Map& map) {
        const auto next_x = position_.x() + delta_x;
        const auto next_y = position_.y() + delta_y;

        if (!map.is_solid_at(static_cast<int>(next_x), static_cast<int>(position_.y()))) {
            position_.set_x(next_x);
        }
        if (!map.is_solid_at(static_cast<int>(position_.x()), static_cast<int>(next_y))) {
            position_.set_y(next_y);
        }
    }

    void Player::try_move_forward(float delta_time_seconds, const Map& map) {
        const auto step = move_speed_ * delta_time_seconds;
        try_translate(direction_.x() * step, direction_.y() * step, map);
    }

    void Player::try_move_backward(float delta_time_seconds, const Map& map) {
        const auto step = move_speed_ * delta_time_seconds;
        try_translate(-direction_.x() * step, -direction_.y() * step, map);
    }

    void Player::try_strafe_left(float delta_time_seconds, const Map& map) {
        const auto step = move_speed_ * delta_time_seconds;
        try_translate(-camera_plane_.x() * step, -camera_plane_.y() * step, map);
    }

    void Player::try_strafe_right(float delta_time_seconds, const Map& map) {
        const auto step = move_speed_ * delta_time_seconds;
        try_translate(camera_plane_.x() * step, camera_plane_.y() * step, map);
    }

    void Player::rotate(float angle_radians) {
        const auto cosine = std::cos(angle_radians);
        const auto sine = std::sin(angle_radians);

        const auto new_dir_x = direction_.x() * cosine - direction_.y() * sine;
        const auto new_dir_y = direction_.x() * sine + direction_.y() * cosine;
        direction_.set_x(new_dir_x);
        direction_.set_y(new_dir_y);

        const auto new_plane_x = camera_plane_.x() * cosine - camera_plane_.y() * sine;
        const auto new_plane_y = camera_plane_.x() * sine + camera_plane_.y() * cosine;
        camera_plane_.set_x(new_plane_x);
        camera_plane_.set_y(new_plane_y);
    }

    void Player::rotate_left(float delta_time_seconds) {
        rotate(-rotation_speed_ * delta_time_seconds);
    }

    void Player::rotate_right(float delta_time_seconds) {
        rotate(rotation_speed_ * delta_time_seconds);
    }

    auto operator<<(std::ostream& stream, const Player& player) -> std::ostream& {
        stream << "Player{position=" << player.position()
               << ", direction=" << player.direction()
               << ", camera_plane=" << player.camera_plane()
               << ", move_speed=" << player.move_speed()
               << ", rotation_speed=" << player.rotation_speed() << "}";
        return stream;
    }
}
