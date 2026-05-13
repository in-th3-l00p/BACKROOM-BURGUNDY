#include "GameEngine.hpp"

#include <chrono>
#include <ostream>

#include <SDL3/SDL.h>

namespace escape::app {
    namespace {
        constexpr float default_move_speed = 3.0F;
        constexpr float default_rotation_speed = 2.5F;
    }

    GameEngine::GameEngine(WindowConfig config)
        : window_(config),
          map_(game::Map::build_demo_map()),
          player_(game::Vector2 {12.0F, 12.0F},
                  game::Vector2 {-1.0F, 0.0F},
                  game::Vector2 {0.0F, 0.66F},
                  default_move_speed,
                  default_rotation_speed),
          raycaster_(config.width, config.height) {}

    void GameEngine::run() {
        auto previous_frame_time = std::chrono::steady_clock::now();

        while (window_.is_open()) {
            const auto current_frame_time = std::chrono::steady_clock::now();
            const auto delta_time_seconds =
                std::chrono::duration<float> {current_frame_time - previous_frame_time}.count();
            previous_frame_time = current_frame_time;

            window_.process_events();
            handle_input(delta_time_seconds);
            render();
        }
    }

    void GameEngine::handle_input(float delta_time_seconds) {
        SDL_PumpEvents();
        const bool* keyboard_state = SDL_GetKeyboardState(nullptr);

        if (keyboard_state[SDL_SCANCODE_W]) {
            player_.try_move_forward(delta_time_seconds, map_);
        }
        if (keyboard_state[SDL_SCANCODE_S]) {
            player_.try_move_backward(delta_time_seconds, map_);
        }
        if (keyboard_state[SDL_SCANCODE_A]) {
            player_.try_strafe_left(delta_time_seconds, map_);
        }
        if (keyboard_state[SDL_SCANCODE_D]) {
            player_.try_strafe_right(delta_time_seconds, map_);
        }
        if (keyboard_state[SDL_SCANCODE_LEFT]) {
            player_.rotate_left(delta_time_seconds);
        }
        if (keyboard_state[SDL_SCANCODE_RIGHT]) {
            player_.rotate_right(delta_time_seconds);
        }
    }

    void GameEngine::render() {
        window_.clear(Color {.r = 0, .g = 0, .b = 0, .a = 255});
        raycaster_.render(player_, map_, window_);
        window_.present();
    }

    auto operator<<(std::ostream& stream, const GameEngine& game_engine) -> std::ostream& {
        stream << "GameEngine{window=" << game_engine.window_
               << ", map=" << game_engine.map_
               << ", player=" << game_engine.player_
               << ", raycaster=" << game_engine.raycaster_ << "}";
        return stream;
    }
}
