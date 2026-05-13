#pragma once

#include "GameDirector.hpp"

#include <iosfwd>
#include <string>

namespace escape::game {
    class Map;
}

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
            SceneSpawn spawn;
            int map_width {0};
            int map_height {0};
            std::string title;
            std::string intro;
        };

        // Populates an existing GameDirector and Map from a JSON file.
        // The JSON schema is documented in assets/burgundy/scene.example.json.
        static auto load_from_file(const std::string& path,
                                   GameDirector& director,
                                   game::Map& map) -> LoadResult;
    };

    auto operator<<(std::ostream& stream, const SceneLoader::LoadResult& result) -> std::ostream&;
}
