#pragma once

#include <memory>
#include <stdexcept>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ComponentStorage.hpp"
#include "Entity.hpp"

namespace escape::ecs {
    class Registry {
    public:
        Registry() = default;
        ~Registry() = default;

        Registry(const Registry&) = delete;
        Registry(Registry&&) noexcept = default;
        auto operator=(const Registry&) -> Registry& = delete;
        auto operator=(Registry&&) noexcept -> Registry& = default;

        auto create_entity() -> Entity {
            if (!available_entities_.empty()) {
                const auto entity = available_entities_.back();
                available_entities_.pop_back();
                living_entities_[entity] = true;
                return entity;
            }

            const auto entity = next_entity_++;
            living_entities_[entity] = true;
            return entity;
        }

        void destroy_entity(Entity entity) {
            if (!is_alive(entity)) {
                return;
            }

            for (auto& [_, storage] : component_storages_) {
                storage->erase(entity);
            }

            living_entities_[entity] = false;
            available_entities_.push_back(entity);
        }

        template <typename Component, typename... Args>
        auto emplace(Entity entity, Args&&... args) -> Component& {
            ensure_entity_is_alive(entity);
            return storage<Component>().emplace(entity, std::forward<Args>(args)...);
        }

        template <typename Component>
        auto has(Entity entity) const -> bool {
            const auto* component_storage = find_storage<Component>();
            return component_storage != nullptr && component_storage->contains(entity);
        }

        template <typename Component>
        auto get(Entity entity) -> Component& {
            ensure_entity_is_alive(entity);
            return storage<Component>().get(entity);
        }

        template <typename Component>
        auto get(Entity entity) const -> const Component& {
            ensure_entity_is_alive(entity);
            return storage<Component>().get(entity);
        }

        template <typename Component>
        void remove(Entity entity) {
            if (auto* component_storage = find_storage<Component>()) {
                component_storage->erase(entity);
            }
        }

        template <typename... Components, typename Func>
        void each(Func&& func) {
            static_assert(sizeof...(Components) > 0, "each requires at least one component");

            auto& primary = storage<std::tuple_element_t<0, std::tuple<Components...>>>().data();

            for (auto& [entity, component] : primary) {
                if ((has<Components>(entity) && ...)) {
                    std::forward<Func>(func)(entity, get<Components>(entity)...);
                }
            }
        }

        void clear() noexcept {
            for (auto& [_, storage] : component_storages_) {
                storage->clear();
            }

            component_storages_.clear();
            available_entities_.clear();
            living_entities_.clear();
            next_entity_ = 1;
        }

        auto is_alive(Entity entity) const -> bool {
            const auto iterator = living_entities_.find(entity);
            return iterator != living_entities_.end() && iterator->second;
        }

    private:
        template <typename Component>
        auto storage() -> ComponentStorage<Component>& {
            const auto type = std::type_index {typeid(Component)};
            auto iterator = component_storages_.find(type);

            if (iterator == component_storages_.end()) {
                auto component_storage = std::make_unique<ComponentStorage<Component>>();
                auto [inserted_iterator, inserted] =
                    component_storages_.emplace(type, std::move(component_storage));
                static_cast<void>(inserted);
                iterator = inserted_iterator;
            }

            return static_cast<ComponentStorage<Component>&>(*iterator->second);
        }

        template <typename Component>
        auto storage() const -> const ComponentStorage<Component>& {
            const auto* component_storage = find_storage<Component>();

            if (component_storage == nullptr) {
                throw std::out_of_range {"Requested component storage does not exist"};
            }

            return *component_storage;
        }

        template <typename Component>
        auto find_storage() -> ComponentStorage<Component>* {
            const auto iterator = component_storages_.find(std::type_index {typeid(Component)});
            if (iterator == component_storages_.end()) {
                return nullptr;
            }

            return static_cast<ComponentStorage<Component>*>(iterator->second.get());
        }

        template <typename Component>
        auto find_storage() const -> const ComponentStorage<Component>* {
            const auto iterator = component_storages_.find(std::type_index {typeid(Component)});
            if (iterator == component_storages_.end()) {
                return nullptr;
            }

            return static_cast<const ComponentStorage<Component>*>(iterator->second.get());
        }

        void ensure_entity_is_alive(Entity entity) const {
            if (!is_alive(entity)) {
                throw std::out_of_range {"Requested entity is not alive in the registry"};
            }
        }

        Entity next_entity_ {1};
        std::unordered_map<Entity, bool> living_entities_ {};
        std::vector<Entity> available_entities_ {};
        std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> component_storages_ {};
    };
} 
