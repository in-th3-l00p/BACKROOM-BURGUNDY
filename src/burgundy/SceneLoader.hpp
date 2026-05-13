#pragma once

#include "../game/Map.hpp"
#include "GameDirector.hpp"

#include <iosfwd>
#include <optional>
#include <string>

namespace escape::burgundy {
    struct SceneSpawn {
        float x {12.0F};
        float y {12.0F};
        float dir_x {-1.0F};
        float dir_y {0.0F};
    };

    class SceneLoader {
    public:
        struct LoadResult {
            game::Map map;
            SceneSpawn spawn;
            std::string title;
            std::string intro;
        };

        // Build a Map and populate a GameDirector from a JSON scene file.
        // The JSON schema is documented in assets/burgundy/scene.example.json.
        static auto load_from_file(const std::string& path, GameDirector& director) -> LoadResult;
    };

    auto operator<<(std::ostream& stream, const SceneLoader::LoadResult& result) -> std::ostream&;
}
