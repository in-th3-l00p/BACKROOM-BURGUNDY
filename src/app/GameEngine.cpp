#include "GameEngine.hpp"

#include "../game/Texture.hpp"

#include <chrono>
#include <memory>
#include <ostream>

#include <SDL3/SDL.h>

namespace escape::app {
    namespace {
        constexpr float default_move_speed = 3.0F;
        constexpr float default_rotation_speed = 2.5F;
        constexpr int internal_resolution_width = 640;
        constexpr int internal_resolution_height = 400;
    }

    GameEngine::GameEngine(WindowConfig config)
        : window_(config),
          framebuffer_(internal_resolution_width, internal_resolution_height),
          map_(game::Map::build_demo_map()),
          player_(game::Vector2 {12.0F, 12.0F},
                  game::Vector2 {-1.0F, 0.0F},
                  game::Vector2 {0.0F, 0.66F},
                  default_move_speed,
                  default_rotation_speed),
          raycaster_(internal_resolution_width, internal_resolution_height) {
        raycaster_.set_floor_texture(std::make_shared<game::Texture>(
            game::Texture::make_checker(64, 64,
                Color {.r = 120, .g = 120, .b = 120, .a = 255},
                Color {.r = 70,  .g = 70,  .b = 70,  .a = 255},
                8)));
        raycaster_.set_ceiling_texture(std::make_shared<game::Texture>(
            game::Texture::make_gradient(64, 64,
                Color {.r = 20, .g = 20, .b = 40, .a = 255},
                Color {.r = 60, .g = 60, .b = 90, .a = 255})));
    }

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
        raycaster_.render(player_, map_, framebuffer_);
        window_.present_framebuffer(framebuffer_);
    }

    auto operator<<(std::ostream& stream, const GameEngine& game_engine) -> std::ostream& {
        stream << "GameEngine{window=" << game_engine.window_
               << ", framebuffer=" << game_engine.framebuffer_
               << ", map=" << game_engine.map_
               << ", player=" << game_engine.player_
               << ", raycaster=" << game_engine.raycaster_ << "}";
        return stream;
    }
}
