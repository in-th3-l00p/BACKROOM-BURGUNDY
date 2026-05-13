#include "Inventory.hpp"

#include <algorithm>
#include <ostream>
#include <utility>

namespace escape::burgundy {
    void Inventory::add(Item item) {
        if (!contains(item.id())) {
            items_.push_back(std::move(item));
        }
    }

    auto Inventory::contains(std::string_view id) const -> bool {
        return std::any_of(items_.begin(), items_.end(),
            [&](const Item& item) { return item.id() == id; });
    }

    auto operator<<(std::ostream& stream, const Inventory& inventory) -> std::ostream& {
        stream << "Inventory{size=" << inventory.size() << "}";
        return stream;
    }
}
