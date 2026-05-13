#include "app/GameEngine.hpp"
#include "game/Map.hpp"
#include "game/Player.hpp"
#include "game/Raycaster.hpp"
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
        const auto transparent_style = style.with_alpha(220);

        auto map = escape::game::Map::build_demo_map();
        auto player = escape::game::Player {
            escape::game::Vector2 {12.0F, 12.0F},
            escape::game::Vector2 {-1.0F, 0.0F},
            escape::game::Vector2 {0.0F, 0.66F},
            3.0F,
            2.5F,
        };
        auto copied_player = player;
        auto assigned_player = escape::game::Player {
            escape::game::Vector2 {2.0F, 2.0F},
            escape::game::Vector2 {1.0F, 0.0F},
            escape::game::Vector2 {0.0F, 0.66F},
            2.0F,
            1.5F,
        };
        assigned_player = copied_player;
        auto raycaster = escape::game::Raycaster {320, 200};
        const auto sample_hit = raycaster.cast_ray(160, player, map);

        std::cout << vector << '\n';
        std::cout << translated_vector << '\n';
        std::cout << scaled_vector << '\n';
        std::cout << size << '\n';
        std::cout << style << '\n';
        std::cout << transparent_style << '\n';
        std::cout << map << '\n';
        std::cout << player << '\n';
        std::cout << copied_player << '\n';
        std::cout << assigned_player << '\n';
        std::cout << raycaster << '\n';
        std::cout << "sample ray hit: distance=" << sample_hit.perp_distance
                  << " map_x=" << sample_hit.map_x
                  << " map_y=" << sample_hit.map_y
                  << " side=" << sample_hit.side
                  << " cell=" << sample_hit.cell_value << '\n';

        auto registry = escape::ecs::Registry {};
        const auto entity = registry.create_entity();
        registry.emplace<escape::game::Player>(entity, player);
        std::cout << "registry has player: "
                  << (registry.has<escape::game::Player>(entity) ? "true" : "false") << '\n';
        std::cout << registry.get<escape::game::Player>(entity) << '\n';
        registry.remove<escape::game::Player>(entity);
        registry.destroy_entity(entity);
        registry.clear();

        auto engine = escape::app::GameEngine {escape::app::WindowConfig {"escape engine", 1280, 720}};
        std::cout << engine << '\n';
        engine.run();
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "escape failed to start: " << exception.what() << '\n';
        return 1;
    }
}
