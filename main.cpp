#include "app/Framebuffer.hpp"
#include "app/GameEngine.hpp"
#include "errors/Exceptions.hpp"
#include "game/EmptyTile.hpp"
#include "game/Map.hpp"
#include "game/Player.hpp"
#include "game/Raycaster.hpp"
#include "game/RenderStyle.hpp"
#include "game/Size2D.hpp"
#include "game/SolidWallTile.hpp"
#include "game/Sprite.hpp"
#include "game/SpriteRenderer.hpp"
#include "game/Texture.hpp"
#include "game/TexturedWallTile.hpp"
#include "game/Tile.hpp"
#include "game/Vector2.hpp"

#include "ecs/Registry.hpp"

#include <exception>
#include <iostream>
#include <memory>

namespace {
    void demonstrate_exception_hierarchy() {
        try {
            const auto invalid_texture = escape::game::Texture {-1, -1, {}};
            std::cout << invalid_texture << '\n';
        } catch (const escape::errors::InvalidConfigError& error) {
            std::cout << "caught InvalidConfigError: " << error.what() << '\n';
        }

        try {
            static_cast<void>(escape::game::Texture::load_from_ppm("missing-texture.ppm"));
        } catch (const escape::errors::TextureLoadError& error) {
            std::cout << "caught TextureLoadError: " << error.what() << '\n';
        }

        try {
            auto map = escape::game::Map::build_demo_map();
            static_cast<void>(map.tile_at(-1, -1));
        } catch (const escape::errors::OutOfBoundsError& error) {
            std::cout << "caught OutOfBoundsError: " << error.what() << '\n';
        }

        std::cout << "Texture::invalid_texture_count="
                  << escape::game::Texture::invalid_texture_count() << '\n';
    }
}

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
        const auto map_copy = map;

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

        const auto brick_texture = std::make_shared<escape::game::Texture>(
            escape::game::Texture::make_brick(64, 64,
                escape::app::Color {.r = 200, .g = 80, .b = 60, .a = 255},
                escape::app::Color {.r = 40,  .g = 30, .b = 20, .a = 255}));
        auto sprite = escape::game::Sprite {
            "preview-sprite", escape::game::Vector2 {3.5F, 3.5F}, brick_texture, 1.0F,
        };
        auto sprite_renderer = escape::game::SpriteRenderer {320, 200};

        std::unique_ptr<escape::game::Tile> empty_tile = std::make_unique<escape::game::EmptyTile>();
        std::unique_ptr<escape::game::Tile> solid_tile = std::make_unique<escape::game::SolidWallTile>(
            7, escape::app::Color {.r = 200, .g = 60, .b = 90, .a = 255});
        std::unique_ptr<escape::game::Tile> textured_tile = std::make_unique<escape::game::TexturedWallTile>(
            8, brick_texture);

        std::cout << vector << '\n';
        std::cout << translated_vector << '\n';
        std::cout << scaled_vector << '\n';
        std::cout << size << '\n';
        std::cout << style << '\n';
        std::cout << transparent_style << '\n';
        std::cout << map << '\n';
        std::cout << map_copy << '\n';
        std::cout << player << '\n';
        std::cout << copied_player << '\n';
        std::cout << assigned_player << '\n';
        std::cout << raycaster << '\n';
        std::cout << sprite << '\n';
        std::cout << sprite_renderer << '\n';
        std::cout << *empty_tile << '\n';
        std::cout << *solid_tile << '\n';
        std::cout << *textured_tile << '\n';
        std::cout << *brick_texture << '\n';
        std::cout << "sample ray hit: distance=" << sample_hit.perp_distance
                  << " map_x=" << sample_hit.map_x
                  << " map_y=" << sample_hit.map_y
                  << " side=" << sample_hit.side
                  << " cell=" << sample_hit.cell_value << '\n';

        demonstrate_exception_hierarchy();

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
    } catch (const escape::errors::EscapeError& error) {
        std::cerr << "escape error: " << error.what() << '\n';
        return 2;
    } catch (const std::exception& exception) {
        std::cerr << "escape failed to start: " << exception.what() << '\n';
        return 1;
    }
}
