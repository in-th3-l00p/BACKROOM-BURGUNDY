#include "GameEngine.hpp"

#include "../game/PlayerRectangle.hpp"
#include "../game/RenderStyle.hpp"
#include "../game/Size2D.hpp"
#include "../game/Vector2.hpp"

#include <chrono>
#include <ostream>

#include <SDL3/SDL.h>

namespace escape::app {
    struct MoveSpeed {
        explicit MoveSpeed(float units_per_second_value)
            : units_per_second(units_per_second_value) {}

        float units_per_second {0.0F};
    };

    GameEngine::GameEngine(WindowConfig config)
        : window_(std::move(config)) {}

    void GameEngine::run() {
        bootstrap_demo_scene();
        auto previous_frame_time = std::chrono::steady_clock::now();

        while (window_.is_open()) {
            const auto current_frame_time = std::chrono::steady_clock::now();
            const auto delta_time_seconds =
                std::chrono::duration<float> {current_frame_time - previous_frame_time}.count();
            previous_frame_time = current_frame_time;

            window_.process_events();
            handle_input(delta_time_seconds);
            update(delta_time_seconds);
            render();
        }
    }

    void GameEngine::bootstrap_demo_scene() {
        const auto entity = registry_.create_entity();

        registry_.emplace<game::PlayerRectangle>(
            entity,
            "player",
            game::Vector2 {200.0F, 160.0F},
            game::Size2D {140.0F, 100.0F},
            game::RenderStyle {Color {.r = 247, .g = 120, .b = 37, .a = 255}});
        registry_.emplace<MoveSpeed>(entity, 320.0F);
    }

    void GameEngine::handle_input(float delta_time_seconds) {
        SDL_PumpEvents();
        const bool* keyboard_state = SDL_GetKeyboardState(nullptr);

        registry_.each<game::PlayerRectangle, MoveSpeed>([&](ecs::Entity, game::PlayerRectangle& player, const MoveSpeed& speed) {
            auto horizontal_axis = 0.0F;
            auto vertical_axis = 0.0F;

            if (keyboard_state[SDL_SCANCODE_A]) {
                horizontal_axis -= 1.0F;
            }
            if (keyboard_state[SDL_SCANCODE_D]) {
                horizontal_axis += 1.0F;
            }
            if (keyboard_state[SDL_SCANCODE_W]) {
                vertical_axis -= 1.0F;
            }
            if (keyboard_state[SDL_SCANCODE_S]) {
                vertical_axis += 1.0F;
            }

            player.move_with_direction(horizontal_axis, vertical_axis, speed.units_per_second, delta_time_seconds);
            player.clamp_to_bounds(game::Size2D {
                static_cast<float>(window_.width()),
                static_cast<float>(window_.height()),
            });
        });
    }

    void GameEngine::update(float delta_time_seconds) {
        static_cast<void>(delta_time_seconds);
    }

    void GameEngine::render() {
        window_.clear(Color {.r = 18, .g = 18, .b = 24, .a = 255});

        registry_.each<game::PlayerRectangle>([&](ecs::Entity, const game::PlayerRectangle& player) {
            window_.draw_filled_rect(player.to_window_rectangle(), player.style().fill_color());
        });

        window_.present();
    }

    auto operator<<(std::ostream& stream, const GameEngine& game_engine) -> std::ostream& {
        stream << "GameEngine{window=" << game_engine.window_ << "}";
        return stream;
    }
}
