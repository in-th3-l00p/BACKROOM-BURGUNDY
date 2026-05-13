#include "GameEngine.hpp"

#include "../burgundy/Interactable.hpp"
#include "../burgundy/SceneLoader.hpp"
#include "../errors/Exceptions.hpp"
#include "../game/DoorTile.hpp"
#include "../game/Texture.hpp"
#include "../patterns/TileFactory.hpp"
#include "../patterns/WallShadingStrategy.hpp"

#include <imgui.h>

#include <chrono>
#include <cmath>
#include <memory>
#include <ostream>
#include <utility>

#include <SDL3/SDL.h>

namespace escape::app {
    namespace {
        constexpr float default_move_speed = 3.0F;
        constexpr float default_rotation_speed = 2.5F;
        constexpr int internal_resolution_width = 640;
        constexpr int internal_resolution_height = 400;

        auto load_scene(const std::string& path, burgundy::GameDirector& director) -> burgundy::SceneLoader::LoadResult {
            return burgundy::SceneLoader::load_from_file(path, director);
        }
    }

    GameEngine::GameEngine(GameEngineConfig config)
        : window_(config.window),
          framebuffer_(internal_resolution_width, internal_resolution_height),
          imgui_(window_.sdl_window(), window_.sdl_renderer()),
          director_(),
          scene_(load_scene(config.scene_path, director_)),
          map_(std::move(scene_.map)),
          player_(game::Vector2 {scene_.spawn.x, scene_.spawn.y},
                  game::Vector2 {scene_.spawn.dir_x, scene_.spawn.dir_y},
                  game::Vector2 {-scene_.spawn.dir_y * 0.66F, scene_.spawn.dir_x * 0.66F},
                  default_move_speed,
                  default_rotation_speed),
          raycaster_(internal_resolution_width, internal_resolution_height),
          sprite_renderer_(internal_resolution_width, internal_resolution_height),
          fps_target_(60.0F, 30.0F, 240.0F),
          player_inventory_(0, 0, 99) {
        scene_title_ = scene_.title;
        scene_intro_ = scene_.intro;

        raycaster_.set_floor_texture(std::make_shared<game::Texture>(
            game::Texture::make_checker(64, 64,
                Color {.r = 80, .g = 60, .b = 35, .a = 255},
                Color {.r = 40, .g = 25, .b = 18, .a = 255},
                8)));
        raycaster_.set_ceiling_texture(std::make_shared<game::Texture>(
            game::Texture::make_gradient(64, 64,
                Color {.r = 22, .g = 12, .b = 16, .a = 255},
                Color {.r = 60, .g = 30, .b = 25, .a = 255})));
        raycaster_.set_shading_strategy(std::make_unique<patterns::FogShading>(
            Color {.r = 25, .g = 12, .b = 14, .a = 255}, 0.10F));

        window_.set_event_callback(&GameEngine::forward_event, this);
    }

    void GameEngine::forward_event(void* user_data, const SDL_Event& event) {
        auto* engine = static_cast<GameEngine*>(user_data);
        engine->imgui_.process_event(event);
    }

    auto GameEngine::find_door_in_front() -> std::pair<int, int> {
        const auto pos_x = player_.position().x();
        const auto pos_y = player_.position().y();
        const auto dir_x = player_.direction().x();
        const auto dir_y = player_.direction().y();

        for (float step = 0.5F; step < 2.0F; step += 0.5F) {
            const int target_x = static_cast<int>(pos_x + dir_x * step);
            const int target_y = static_cast<int>(pos_y + dir_y * step);
            if (!map_.in_bounds(target_x, target_y)) {
                continue;
            }
            if (dynamic_cast<const game::DoorTile*>(&map_.tile_at(target_x, target_y)) != nullptr) {
                return {target_x, target_y};
            }
        }
        return {-1, -1};
    }

    auto GameEngine::find_interactable_tile_in_front() -> std::pair<int, int> {
        const auto pos_x = player_.position().x();
        const auto pos_y = player_.position().y();
        const auto dir_x = player_.direction().x();
        const auto dir_y = player_.direction().y();

        for (float step = 0.5F; step < 2.5F; step += 0.5F) {
            const int target_x = static_cast<int>(pos_x + dir_x * step);
            const int target_y = static_cast<int>(pos_y + dir_y * step);
            if (director_.interactable_at(target_x, target_y) != nullptr) {
                return {target_x, target_y};
            }
        }
        return {-1, -1};
    }

    void GameEngine::try_interact() {
        const auto [x, y] = find_interactable_tile_in_front();
        if (x < 0) {
            director_.notify("Nothing within reach.");
            return;
        }
        auto* interactable = director_.interactable_at(x, y);
        if (interactable == nullptr) {
            return;
        }
        interactable->interact(director_);
        if (interactable->is_consumed()) {
            director_.consume_interactable_at(x, y);
        }
    }

    void GameEngine::advance_doors(float delta_time_seconds) {
        for (int y = 0; y < map_.height(); ++y) {
            for (int x = 0; x < map_.width(); ++x) {
                if (auto* door = const_cast<game::DoorTile*>(
                        dynamic_cast<const game::DoorTile*>(&map_.tile_at(x, y)))) {
                    door->advance(delta_time_seconds);
                }
            }
        }
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
            advance_doors(delta_time_seconds);
            render();
        }
    }

    void GameEngine::handle_input(float delta_time_seconds) {
        if (director_.active_puzzle() != nullptr) {
            return; // freeze world while a puzzle modal is open
        }

        SDL_PumpEvents();
        const bool* keyboard_state = SDL_GetKeyboardState(nullptr);

        if (imgui_.wants_keyboard()) {
            return;
        }

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
            player_.rotate_right(delta_time_seconds);
        }
        if (keyboard_state[SDL_SCANCODE_RIGHT]) {
            player_.rotate_left(delta_time_seconds);
        }

        const bool space_pressed = keyboard_state[SDL_SCANCODE_SPACE];
        if (space_pressed && !space_was_pressed_) {
            const auto [door_x, door_y] = find_door_in_front();
            if (door_x >= 0) {
                auto* door = const_cast<game::DoorTile*>(
                    dynamic_cast<const game::DoorTile*>(&map_.tile_at(door_x, door_y)));
                if (door != nullptr) {
                    door->toggle();
                    player_inventory_.add(1);
                }
            }
        }
        space_was_pressed_ = space_pressed;

        const bool interact_pressed = keyboard_state[SDL_SCANCODE_E];
        if (interact_pressed && !interact_was_pressed_) {
            try_interact();
        }
        interact_was_pressed_ = interact_pressed;
    }

    void GameEngine::render() {
        raycaster_.render(player_, map_, framebuffer_);

        auto sprite_pointers = std::vector<const game::Sprite*> {};
        sprite_pointers.reserve(sprites_.size());
        for (const auto& sprite : sprites_) {
            sprite_pointers.push_back(&sprite);
        }
        sprite_renderer_.render(player_, sprite_pointers, raycaster_, framebuffer_);

        window_.clear(Color {.r = 0, .g = 0, .b = 0, .a = 255});
        window_.blit_framebuffer(framebuffer_);

        imgui_.begin_frame();
        draw_overlay_ui();
        imgui_.end_frame(window_.sdl_renderer());

        window_.swap();
    }

    void GameEngine::draw_overlay_ui() {
        ImGui::SetNextWindowPos(ImVec2 {12, 12}, ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2 {320, 96}, ImGuiCond_Once);
        ImGui::Begin("Burgundy Room", nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::TextColored(ImVec4 {0.95F, 0.78F, 0.55F, 1.0F}, "%s", scene_title_.c_str());
        ImGui::TextWrapped("%s", director_.status_line().c_str());
        ImGui::Separator();
        ImGui::Text("W/A/S/D move - arrows turn - E interact");
        ImGui::Checkbox("Inventory", &show_inventory_);
        ImGui::SameLine();
        ImGui::Checkbox("Narrative", &show_narrative_);
        ImGui::SameLine();
        ImGui::Checkbox("Help", &show_help_);
        if (director_.is_won()) {
            ImGui::TextColored(ImVec4 {0.6F, 1.0F, 0.7F, 1.0F}, "ESCAPED.");
        }
        ImGui::End();

        if (show_inventory_) {
            ImGui::SetNextWindowPos(ImVec2 {12, 130}, ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2 {320, 220}, ImGuiCond_Once);
            ImGui::Begin("Inventory", &show_inventory_);
            if (director_.inventory().items().empty()) {
                ImGui::TextDisabled("Empty.");
            }
            for (const auto& item : director_.inventory().items()) {
                if (ImGui::TreeNode(item.title().c_str())) {
                    ImGui::TextWrapped("%s", item.description().c_str());
                    ImGui::TreePop();
                }
            }
            ImGui::End();
        }

        if (show_narrative_) {
            ImGui::SetNextWindowPos(ImVec2 {12, 360}, ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2 {420, 220}, ImGuiCond_Once);
            ImGui::Begin("Narrative log", &show_narrative_);
            for (const auto& line : director_.narrative_log()) {
                ImGui::TextWrapped("- %s", line.c_str());
            }
            ImGui::SetScrollHereY(1.0F);
            ImGui::End();
        }

        if (show_help_) {
            ImGui::SetNextWindowPos(ImVec2 {ImGui::GetIO().DisplaySize.x - 340, 12}, ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2 {320, 240}, ImGuiCond_Once);
            ImGui::Begin("How to play", &show_help_);
            ImGui::TextWrapped("%s", scene_intro_.c_str());
            ImGui::Separator();
            ImGui::TextWrapped("Walk close to objects and press E. Solve all puzzles to find the safe key, then leave through the back door.");
            ImGui::End();
        }

        if (auto* puzzle = director_.active_puzzle(); puzzle != nullptr) {
            const auto display = ImGui::GetIO().DisplaySize;
            ImGui::SetNextWindowPos(ImVec2 {display.x * 0.5F, display.y * 0.5F}, ImGuiCond_Always, ImVec2 {0.5F, 0.5F});
            ImGui::SetNextWindowSize(ImVec2 {460, 320}, ImGuiCond_Once);
            ImGui::Begin(puzzle->title().c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
            puzzle->draw_ui();
            ImGui::Separator();
            if (ImGui::Button("Close")) {
                director_.close_puzzle();
            }
            if (puzzle->is_solved() && !puzzle->reward_item_id().empty()) {
                if (auto* item_template = director_.item_template(puzzle->reward_item_id())) {
                    director_.grant_item(item_template->id(), item_template->title(), item_template->description());
                } else {
                    director_.grant_item_by_id(puzzle->reward_item_id());
                }
                if (!puzzle->reward_message().empty()) {
                    director_.notify(puzzle->reward_message());
                }
                puzzle->set_reward_item_id("");
            }
            ImGui::End();
        }
    }

    auto operator<<(std::ostream& stream, const GameEngine& game_engine) -> std::ostream& {
        stream << "GameEngine{window=" << game_engine.window_
               << ", framebuffer=" << game_engine.framebuffer_
               << ", map=" << game_engine.map_
               << ", player=" << game_engine.player_
               << ", raycaster=" << game_engine.raycaster_
               << ", sprites=" << game_engine.sprites_.size()
               << ", director=" << game_engine.director_ << "}";
        return stream;
    }
}
