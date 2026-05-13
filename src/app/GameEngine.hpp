#pragma once

#include "../burgundy/GameDirector.hpp"
#include "../burgundy/SceneLoader.hpp"
#include "../ecs/Registry.hpp"
#include "../game/Map.hpp"
#include "../game/Player.hpp"
#include "../game/Raycaster.hpp"
#include "../game/Sprite.hpp"
#include "../game/SpriteRenderer.hpp"
#include "../templates/Bounded.hpp"
#include "Framebuffer.hpp"
#include "ImGuiOverlay.hpp"
#include "Window.hpp"

#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace escape::app {
    struct GameEngineConfig {
        WindowConfig window {};
        std::string scene_path {"assets/burgundy/scene.json"};
    };

    class GameEngine {
    public:
        explicit GameEngine(GameEngineConfig config = GameEngineConfig {});
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
        auto director() const noexcept -> const burgundy::GameDirector& { return director_; }

    private:
        friend auto operator<<(std::ostream& stream, const GameEngine& game_engine) -> std::ostream&;

        static void forward_event(void* user_data, const SDL_Event& event);

        void handle_input(float delta_time_seconds);
        void advance_doors(float delta_time_seconds);
        auto find_door_in_front() -> std::pair<int, int>;
        auto find_interactable_tile_in_front() -> std::pair<int, int>;
        void try_interact();
        void render();
        void draw_overlay_ui();

        Window window_;
        Framebuffer framebuffer_;
        ImGuiOverlay imgui_;
        ecs::Registry registry_ {};
        burgundy::GameDirector director_ {};
        burgundy::SceneLoader::LoadResult scene_;
        game::Map map_;
        game::Player player_;
        game::Raycaster raycaster_;
        game::SpriteRenderer sprite_renderer_;
        std::vector<game::Sprite> sprites_ {};
        templates::Bounded<float> fps_target_;
        templates::Bounded<int> player_inventory_;
        std::string scene_title_ {};
        std::string scene_intro_ {};
        bool space_was_pressed_ {false};
        bool interact_was_pressed_ {false};
        bool show_inventory_ {true};
        bool show_narrative_ {true};
        bool show_help_ {false};
    };

    auto operator<<(std::ostream& stream, const GameEngine& game_engine) -> std::ostream&;
}
