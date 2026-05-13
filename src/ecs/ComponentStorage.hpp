#pragma once

#include <unordered_map>
#include <utility>

#include "Entity.hpp"

namespace escape::ecs {
    class IComponentStorage {
    public:
        virtual ~IComponentStorage() = default;

        virtual void erase(Entity entity) = 0;
        virtual void clear() noexcept = 0;
    };

    template <typename Component>
    class ComponentStorage final : public IComponentStorage {
    public:
        using Storage = std::unordered_map<Entity, Component>;

        auto contains(Entity entity) const -> bool {
            return components_.find(entity) != components_.end();
        }

        template <typename... Args>
        auto emplace(Entity entity, Args&&... args) -> Component& {
            auto [iterator, inserted] =
                components_.try_emplace(entity, std::forward<Args>(args)...);

            if (!inserted) {
                iterator->second = Component {std::forward<Args>(args)...};
            }

            return iterator->second;
        }

        auto get(Entity entity) -> Component& {
            return components_.at(entity);
        }

        auto get(Entity entity) const -> const Component& {
            return components_.at(entity);
        }

        void erase(Entity entity) override {
            components_.erase(entity);
        }

        void clear() noexcept override {
            components_.clear();
        }

        auto data() -> Storage& {
            return components_;
        }

        auto data() const -> const Storage& {
            return components_;
        }

    private:
        Storage components_ {};
    };
}
