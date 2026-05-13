#include "PlayerRectangle.hpp"

#include <ostream>
#include <utility>

namespace escape::game {
    PlayerRectangle::PlayerRectangle(std::string name, Vector2 position, Size2D size, RenderStyle style)
        : name_(std::move(name)),
        position_(position),
        size_(size),
        style_(style) {}

    PlayerRectangle::PlayerRectangle(const PlayerRectangle& other)
        : name_(other.name_),
        position_(other.position_),
        size_(other.size_),
        style_(other.style_) {}

    auto PlayerRectangle::operator=(const PlayerRectangle& other) -> PlayerRectangle& {
        if (this == &other) {
            return *this;
        }

        name_ = other.name_;
        position_ = other.position_;
        size_ = other.size_;
        style_ = other.style_;
        return *this;
    }

    PlayerRectangle::~PlayerRectangle() = default;

    auto PlayerRectangle::name() const -> const std::string& {
        return name_;
    }

    auto PlayerRectangle::position() const -> const Vector2& {
        return position_;
    }

    auto PlayerRectangle::size() const -> const Size2D& {
        return size_;
    }

    auto PlayerRectangle::style() const -> const RenderStyle& {
        return style_;
    }

    void PlayerRectangle::move_with_direction(float axis_x, float axis_y, float speed, float delta_time_seconds) {
        position_ = position_.translated(axis_x * speed * delta_time_seconds, axis_y * speed * delta_time_seconds);
    }

    void PlayerRectangle::clamp_to_bounds(const Size2D& bounds) {
        if (position_.x() < 0.0F) {
            position_.set_x(0.0F);
        }
        if (position_.y() < 0.0F) {
            position_.set_y(0.0F);
        }

        const auto max_x = bounds.width() - size_.width();
        const auto max_y = bounds.height() - size_.height();

        if (position_.x() > max_x) {
            position_.set_x(max_x);
        }
        if (position_.y() > max_y) {
            position_.set_y(max_y);
        }
    }

    auto PlayerRectangle::to_window_rectangle() const -> app::Rectangle {
        return app::Rectangle {
            .x = position_.x(),
            .y = position_.y(),
            .width = size_.width(),
            .height = size_.height(),
        };
    }

    auto operator<<(std::ostream& stream, const PlayerRectangle& rectangle) -> std::ostream& {
        stream << "PlayerRectangle{name=" << rectangle.name()
            << ", position=" << rectangle.position()
            << ", size=" << rectangle.size()
            << ", style=" << rectangle.style() << "}";
        return stream;
    }
} 
