#pragma once

#include "../ecs/Registry.hpp"
#include "../game/Map.hpp"
#include "../game/Player.hpp"
#include "../game/Raycaster.hpp"
#include "../game/Sprite.hpp"
#include "../game/SpriteRenderer.hpp"
#include "../templates/Bounded.hpp"
#include "Framebuffer.hpp"
#include "Window.hpp"

#include <utility>
#include <vector>

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

        auto registry() noexcept -> ecs::Registry& { return registry_; }
        auto registry() const noexcept -> const ecs::Registry& { return registry_; }
        auto map() const noexcept -> const game::Map& { return map_; }
        auto player() const noexcept -> const game::Player& { return player_; }

    private:
        friend auto operator<<(std::ostream& stream, const GameEngine& game_engine) -> std::ostream&;

        void handle_input(float delta_time_seconds);
        void advance_doors(float delta_time_seconds);
        auto find_door_in_front() -> std::pair<int, int>;
        void render();

        Window window_;
        Framebuffer framebuffer_;
        ecs::Registry registry_ {};
        game::Map map_;
        game::Player player_;
        game::Raycaster raycaster_;
        game::SpriteRenderer sprite_renderer_;
        std::vector<game::Sprite> sprites_ {};
        templates::Bounded<float> fps_target_;
        templates::Bounded<int> player_inventory_;
        bool space_was_pressed_ {false};
    };

    auto operator<<(std::ostream& stream, const GameEngine& game_engine) -> std::ostream&;
}
