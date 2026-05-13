#pragma once

#include "../ecs/Registry.hpp"
#include "Window.hpp"

#include <iosfwd>

namespace escape::app {
    class GameEngine {
    public:
        explicit GameEngine(WindowConfig config = WindowConfig {"escape engine", 1280, 720});
        ~GameEngine() = default;

        GameEngine(const GameEngine&) = delete;
        GameEngine(GameEngine&&) = delete;
        auto operator=(const GameEngine&) -> GameEngine& = delete;
        auto operator=(GameEngine&&) -> GameEngine& = delete;

        void run();

        auto registry() noexcept -> ecs::Registry& {
            return registry_;
        }

        auto registry() const noexcept -> const ecs::Registry& {
            return registry_;
        }

    private:
        friend auto operator<<(std::ostream& stream, const GameEngine& game_engine) -> std::ostream&;

        void bootstrap_demo_scene();
        void handle_input(float delta_time_seconds);
        void update(float delta_time_seconds);
        void render();

        Window window_;
        ecs::Registry registry_ {};
    };

    auto operator<<(std::ostream& stream, const GameEngine& game_engine) -> std::ostream&;
}
