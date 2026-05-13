#pragma once

#include "../game/Tile.hpp"

#include <functional>
#include <iosfwd>
#include <memory>
#include <unordered_map>

namespace escape::patterns {
    class TileFactory {
    public:
        using Builder = std::function<std::unique_ptr<game::Tile>()>;

        TileFactory();

        void register_builder(int cell_id, Builder builder);
        auto create(int cell_id) const -> std::unique_ptr<game::Tile>;
        auto registered_count() const noexcept -> std::size_t { return builders_.size(); }

        static auto with_demo_registry() -> TileFactory;

    private:
        std::unordered_map<int, Builder> builders_;
    };

    auto operator<<(std::ostream& stream, const TileFactory& factory) -> std::ostream&;
}
