#include "app/GameEngine.hpp"
#include "game/PlayerRectangle.hpp"
#include "game/RenderStyle.hpp"
#include "game/Size2D.hpp"
#include "game/Vector2.hpp"

#include "ecs/Registry.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        auto vector = escape::game::Vector2 {3.0F, 4.0F};
        vector.set_x(5.0F);
        vector.set_y(6.0F);
        const auto translated_vector = vector.translated(-2.0F, 1.0F);
        const auto scaled_vector = translated_vector.scaled(0.5F);

        auto size = escape::game::Size2D {140.0F, 100.0F};
        auto style = escape::game::RenderStyle {escape::app::Color {.r = 247, .g = 120, .b = 37, .a = 255}};
        auto transparent_style = style.with_alpha(220);

        auto player = escape::game::PlayerRectangle {"player", vector, size, style};
        auto copied_player = player;
        auto assigned_player = escape::game::PlayerRectangle {
            "backup",
            escape::game::Vector2 {0.0F, 0.0F},
            escape::game::Size2D {10.0F, 10.0F},
            transparent_style,
        };
        assigned_player = copied_player;
        player.move_with_direction(1.0F, -1.0F, 120.0F, 0.25F);
        player.clamp_to_bounds(escape::game::Size2D {1280.0F, 720.0F});
        const auto screen_rectangle = player.to_window_rectangle();

        std::cout << vector << '\n';
        std::cout << "vector.x=" << vector.x() << ", vector.y=" << vector.y()
                  << ", length_squared=" << vector.length_squared() << '\n';
        std::cout << translated_vector << '\n';
        std::cout << scaled_vector << '\n';
        std::cout << size << ", area=" << size.area()
                  << ", width=" << size.width()
                  << ", height=" << size.height() << '\n';
        std::cout << style << '\n';
        std::cout << transparent_style << '\n';
        std::cout << player << '\n';
        std::cout << copied_player << '\n';
        std::cout << assigned_player << '\n';
        std::cout << "player.name=" << player.name() << '\n';
        std::cout << "player.position=" << player.position() << '\n';
        std::cout << "player.size=" << player.size() << '\n';
        std::cout << "player.style=" << player.style() << '\n';
        std::cout << screen_rectangle << '\n';

        auto registry = escape::ecs::Registry {};
        const auto entity = registry.create_entity();
        registry.emplace<escape::game::PlayerRectangle>(entity, player);
        std::cout << "registry.has<PlayerRectangle>=" << (registry.has<escape::game::PlayerRectangle>(entity) ? "true" : "false") << '\n';
        std::cout << registry.get<escape::game::PlayerRectangle>(entity) << '\n';
        registry.each<escape::game::PlayerRectangle>([](escape::ecs::Entity current_entity, const escape::game::PlayerRectangle& rectangle) {
            std::cout << "entity " << current_entity << ": " << rectangle << '\n';
        });
        registry.remove<escape::game::PlayerRectangle>(entity);
        registry.emplace<escape::game::PlayerRectangle>(entity, assigned_player);
        std::cout << "registry.is_alive=" << (registry.is_alive(entity) ? "true" : "false") << '\n';
        registry.destroy_entity(entity);
        registry.clear();

        auto engine = escape::app::GameEngine {escape::app::WindowConfig {"escape engine", 1280, 720}};
        static_cast<void>(engine.registry());
        std::cout << engine << '\n';
        engine.run();
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "escape failed to start: " << exception.what() << '\n';
        return 1;
    }
}
