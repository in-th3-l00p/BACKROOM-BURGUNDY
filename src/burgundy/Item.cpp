#include "Item.hpp"

#include <ostream>
#include <utility>

namespace escape::burgundy {
    Item::Item(std::string id, std::string title, std::string description)
        : id_(std::move(id)), title_(std::move(title)), description_(std::move(description)) {}

    auto operator<<(std::ostream& stream, const Item& item) -> std::ostream& {
        stream << "Item{id=" << item.id() << ", title=" << item.title() << "}";
        return stream;
    }
}
