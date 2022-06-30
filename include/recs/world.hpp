#pragma once

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <coro/unique_generator.h>
#include <ctti/type_id.hpp>

#include "component_pool.hpp"


namespace recs {
    struct Entity;


    struct World {
        Entity new_entity();

        template <typename Component>
        Component& get(EntityId entity_id) {
            auto& component_pool{get_component_pool<Component>()};
            return component_pool.get(entity_id);
        }

        template <typename Component, typename... Ts>
        Component& replace(EntityId entity_id, Ts&& ... params) {
            auto& component_pool{get_component_pool<Component>()};
            return component_pool.replace(entity_id, std::forward<Ts>(params)...);
        }

        template <typename Component>
        Component& replace(EntityId entity_id, Component&& component) {
            auto& component_pool{get_component_pool<Component>()};
            return component_pool.replace(entity_id, std::forward<Component>(component));
        }

        template <typename... Components>
        bool has(EntityId entity_id) {
            return (... && get_component_pool<Components>().has(entity_id));
        }

        template <typename... Components>
        void del(EntityId entity_id) {
            (..., get_component_pool<Components>().del(entity_id));
        }

        inline void del(EntityId entity_id) {
            for (auto const& [_, pool]: m_pools) {
                pool->del(entity_id);
            }
        }

        template <typename... Components>
        void clear() {
            (..., get_component_pool<Components>().clear());
        }

        template <typename... Components>
        unique_generator<std::tuple<Entity, Components& ...>> iter();

        template <typename... Components, typename Lambda>
        void iter(Lambda lambda);

    private:
        template <typename Component, bool check = true>
        ComponentPool<Component>& get_component_pool() {
            constexpr ctti::detail::hash_t id{ctti::unnamed_type_id<Component>().hash()};

            if constexpr (check) {
                if (!m_pools.contains(id))
                    m_pools[id] = std::make_unique<ComponentPool<Component>>();
            }

            return dynamic_cast<ComponentPool<Component>&>(*m_pools[id]);
        }

        template <typename Component, bool check = true>
        IComponentPool& get_component_pool_base() {
            constexpr ctti::detail::hash_t id{ctti::unnamed_type_id<Component>().hash()};

            if constexpr (check) {
                if (!m_pools.contains(id))
                    m_pools[id] = std::make_unique<ComponentPool<Component>>();
            }

            return *m_pools[id];
        }

    private:
        EntityId m_current_entity_id{0};
        std::unordered_map<ctti::detail::hash_t, std::unique_ptr<IComponentPool>> m_pools{};
    };
}

#include "entity.hpp"
#include "world_impl.hpp"
