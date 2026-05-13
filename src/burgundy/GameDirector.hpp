#pragma once

#include "Interactable.hpp"
#include "Inventory.hpp"
#include "Item.hpp"
#include "Puzzle.hpp"

#include <iosfwd>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace escape::burgundy {
    struct TileKey {
        int x {0};
        int y {0};

        auto operator==(const TileKey& other) const noexcept -> bool {
            return x == other.x && y == other.y;
        }
    };

    struct TileKeyHash {
        auto operator()(const TileKey& key) const noexcept -> std::size_t {
            return static_cast<std::size_t>(key.x) * 73856093U
                 ^ static_cast<std::size_t>(key.y) * 19349663U;
        }
    };

    class GameDirector {
    public:
        GameDirector();

        auto inventory() noexcept -> Inventory& { return inventory_; }
        auto inventory() const noexcept -> const Inventory& { return inventory_; }
        auto narrative_log() const noexcept -> const std::vector<std::string>& { return narrative_log_; }
        auto status_line() const noexcept -> const std::string& { return status_line_; }

        auto active_puzzle() noexcept -> Puzzle* { return active_puzzle_; }
        void open_puzzle(Puzzle* puzzle) noexcept;
        void close_puzzle() noexcept;

        void notify(std::string message);
        void grant_item(std::string id, std::string title, std::string description);
        void grant_item_by_id(const std::string& id);
        void declare_won();
        auto is_won() const noexcept -> bool { return won_; }

        void register_item_template(Item item);
        void register_puzzle(std::unique_ptr<Puzzle> puzzle);
        void register_interactable(int x, int y, std::unique_ptr<Interactable> interactable);

        auto puzzle_by_id(const std::string& id) -> Puzzle*;
        auto interactable_at(int x, int y) -> Interactable*;
        void consume_interactable_at(int x, int y);

        auto item_template(const std::string& id) const -> const Item*;

        auto solved_puzzle_count() const noexcept -> std::size_t;
        auto total_puzzle_count() const noexcept -> std::size_t { return puzzles_.size(); }

    private:
        Inventory inventory_ {};
        std::vector<std::string> narrative_log_ {};
        std::string status_line_ {};
        std::unordered_map<std::string, Item> item_templates_ {};
        std::vector<std::unique_ptr<Puzzle>> puzzles_ {};
        std::unordered_map<TileKey, std::unique_ptr<Interactable>, TileKeyHash> interactables_ {};
        Puzzle* active_puzzle_ {nullptr};
        bool won_ {false};
    };

    auto operator<<(std::ostream& stream, const GameDirector& director) -> std::ostream&;
}
