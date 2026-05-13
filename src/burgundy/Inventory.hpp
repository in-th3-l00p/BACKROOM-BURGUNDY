#pragma once

#include "Item.hpp"

#include <iosfwd>
#include <string_view>
#include <vector>

namespace escape::burgundy {
    class Inventory {
    public:
        void add(Item item);
        auto contains(std::string_view id) const -> bool;
        auto items() const noexcept -> const std::vector<Item>& { return items_; }
        auto size() const noexcept -> std::size_t { return items_.size(); }

    private:
        std::vector<Item> items_ {};
    };

    auto operator<<(std::ostream& stream, const Inventory& inventory) -> std::ostream&;
}
