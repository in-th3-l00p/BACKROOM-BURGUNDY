#include "SceneLoader.hpp"

#include "../errors/Exceptions.hpp"
#include "../game/Map.hpp"
#include "../patterns/TileFactory.hpp"
#include "CombinationPuzzle.hpp"
#include "ExaminePuzzle.hpp"
#include "ExitInteractable.hpp"
#include "KeypadPuzzle.hpp"
#include "PickupInteractable.hpp"
#include "PuzzleInteractable.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <ostream>
#include <utility>

namespace escape::burgundy {
    namespace {
        using nlohmann::json;

        auto make_puzzle(const json& spec) -> std::unique_ptr<Puzzle> {
            const auto kind = spec.value("kind", std::string {});
            auto id = spec.value("id", std::string {});
            auto title = spec.value("title", std::string {});
            auto prompt = spec.value("prompt", std::string {});

            std::unique_ptr<Puzzle> puzzle;
            if (kind == "keypad") {
                auto solution = spec.value("solution", std::string {});
                puzzle = std::make_unique<KeypadPuzzle>(std::move(id), std::move(title),
                    std::move(prompt), std::move(solution));
            } else if (kind == "combination") {
                auto wheel_max = spec.value("wheel_max", 39);
                std::vector<int> solution;
                if (spec.contains("solution") && spec["solution"].is_array()) {
                    for (const auto& value : spec["solution"]) {
                        solution.push_back(value.get<int>());
                    }
                }
                puzzle = std::make_unique<CombinationPuzzle>(std::move(id), std::move(title),
                    std::move(prompt), std::move(solution), wheel_max);
            } else if (kind == "examine") {
                std::vector<std::string> revelations;
                if (spec.contains("revelations") && spec["revelations"].is_array()) {
                    for (const auto& value : spec["revelations"]) {
                        revelations.push_back(value.get<std::string>());
                    }
                }
                puzzle = std::make_unique<ExaminePuzzle>(std::move(id), std::move(title),
                    std::move(prompt), std::move(revelations));
            } else {
                throw errors::MapLoadError {"Unknown puzzle kind: " + kind};
            }

            if (spec.contains("reward_item")) {
                puzzle->set_reward_item_id(spec["reward_item"].get<std::string>());
            }
            if (spec.contains("reward_message")) {
                puzzle->set_reward_message(spec["reward_message"].get<std::string>());
            }
            if (spec.contains("requires") && spec["requires"].is_array()) {
                for (const auto& value : spec["requires"]) {
                    puzzle->add_required_item(value.get<std::string>());
                }
            }
            return puzzle;
        }

        auto make_interactable(const json& spec) -> std::unique_ptr<Interactable> {
            const auto kind = spec.value("kind", std::string {});
            auto label = spec.value("label", std::string {});
            auto narrative = spec.value("narrative", std::string {});

            if (kind == "pickup") {
                auto item_id = spec.value("item", std::string {});
                return std::make_unique<PickupInteractable>(std::move(label),
                    std::move(narrative), std::move(item_id));
            }
            if (kind == "puzzle") {
                auto puzzle_id = spec.value("puzzle", std::string {});
                return std::make_unique<PuzzleInteractable>(std::move(label),
                    std::move(narrative), std::move(puzzle_id));
            }
            if (kind == "exit") {
                auto required_puzzle = spec.value("requires_puzzle", std::string {});
                return std::make_unique<ExitInteractable>(std::move(label),
                    std::move(narrative), std::move(required_puzzle));
            }
            throw errors::MapLoadError {"Unknown interactable kind: " + kind};
        }
    }

    auto SceneLoader::load_from_file(const std::string& path,
                                     GameDirector& director,
                                     game::Map& map) -> LoadResult {
        auto file = std::ifstream {path};
        if (!file.is_open()) {
            throw errors::MapLoadError {"Failed to open scene file: " + path};
        }

        json document;
        try {
            file >> document;
        } catch (const json::parse_error& error) {
            throw errors::MapLoadError {std::string {"Invalid scene JSON: "} + error.what()};
        }

        auto result = LoadResult {};
        result.title = document.value("title", std::string {"Burgundy Room"});
        result.intro = document.value("intro", std::string {});
        result.map_width = map.width();
        result.map_height = map.height();

        if (document.contains("spawn")) {
            const auto& spawn = document["spawn"];
            result.spawn.x = spawn.value("x", result.spawn.x);
            result.spawn.y = spawn.value("y", result.spawn.y);
            result.spawn.dir_x = spawn.value("dir_x", result.spawn.dir_x);
            result.spawn.dir_y = spawn.value("dir_y", result.spawn.dir_y);
        }

        if (document.contains("items") && document["items"].is_array()) {
            for (const auto& spec : document["items"]) {
                auto id = spec.value("id", std::string {});
                auto title = spec.value("title", id);
                auto description = spec.value("description", std::string {});
                if (!id.empty()) {
                    director.register_item_template(Item {std::move(id),
                        std::move(title), std::move(description)});
                }
            }
        }

        if (document.contains("puzzles") && document["puzzles"].is_array()) {
            for (const auto& spec : document["puzzles"]) {
                director.register_puzzle(make_puzzle(spec));
            }
        }

        if (document.contains("tile_overrides") && document["tile_overrides"].is_array()) {
            const auto factory = patterns::TileFactory::with_demo_registry();
            for (const auto& spec : document["tile_overrides"]) {
                const auto x = spec.value("x", -1);
                const auto y = spec.value("y", -1);
                const auto cell_id = spec.value("cell_id", -1);
                if (x < 0 || y < 0 || cell_id < 0) {
                    continue;
                }
                if (map.in_bounds(x, y)) {
                    map.set_tile(x, y, factory.create(cell_id));
                }
            }
        }

        if (document.contains("interactables") && document["interactables"].is_array()) {
            for (const auto& spec : document["interactables"]) {
                const auto x = spec.value("x", -1);
                const auto y = spec.value("y", -1);
                if (x < 0 || y < 0) {
                    continue;
                }
                director.register_interactable(x, y, make_interactable(spec));
            }
        }

        if (document.contains("opening_narrative") && document["opening_narrative"].is_array()) {
            for (const auto& line : document["opening_narrative"]) {
                director.notify(line.get<std::string>());
            }
        }

        return result;
    }

    auto operator<<(std::ostream& stream, const SceneLoader::LoadResult& result) -> std::ostream& {
        stream << "Scene{title=" << result.title
               << ", spawn=(" << result.spawn.x << "," << result.spawn.y << ")"
               << ", map=" << result.map_width << "x" << result.map_height << "}";
        return stream;
    }
}
