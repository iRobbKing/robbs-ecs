#ifndef RECS_WORLD_HPP
#define RECS_WORLD_HPP

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <coro/unique_generator.h>
#include <ctti/type_id.hpp>

#include "detail.hpp"
#include "pool.hpp"


namespace recs {
    struct World {
        struct Entity {
            Entity(EntityId entity_id, World& world)
                    : m_id{entity_id}, m_world{world} {}

            template <typename Component>
            Component& get() {
                return m_world.get<Component>(m_id);
            }

            template <typename Component, typename... Ts>
            Component& replace(Ts&& ... params) {
                return m_world.replace<Component>(m_id, std::forward<Ts>(params)...);
            }

            template <typename Component>
            Component& replace(Component&& component) {
                return m_world.replace(m_id, std::forward<Component>(component));
            }

            template <typename... Components>
            bool has() {
                return m_world.has<Components...>(m_id);
            }

            template <typename... Components>
            void del() {
                m_world.del<Components...>(m_id);
            }

            inline void del() {
                m_world.del(m_id);
            }

            inline EntityId id() const {
                return m_id;
            }

        private:
            EntityId m_id;
            World& m_world;
        };

        Entity new_entity() {
            return {m_current_entity_id++, *this};
        }

        template <typename Component>
        Component& get(EntityId entity_id) {
            auto& pool{component_pool<Component>()};
            return pool.get(entity_id);
        }

        template <typename Component, typename... Ts>
        Component& replace(EntityId entity_id, Ts&& ... params) {
            auto& pool{component_pool<Component>()};
            return pool.replace(entity_id, std::forward<Ts>(params)...);
        }

        template <typename Component>
        Component& replace(EntityId entity_id, Component&& component) {
            auto& pool{component_pool<Component>()};
            return pool.replace(entity_id, std::forward<Component>(component));
        }

        template <typename... Components>
        bool has(EntityId entity_id) {
            return (... && component_pool<Components>().has(entity_id));
        }

        template <typename... Components>
        void del(EntityId entity_id) {
            (..., component_pool<Components>().del(entity_id));
        }

        inline void del(EntityId entity_id) {
            for (auto const& [_, pool]: m_pools) {
                pool->del(entity_id);
            }
        }

        template <typename... Components>
        void clear() {
            (..., component_pool<Components>().clear());
        }

        template <typename... Components>
        unique_generator<std::tuple<Entity, Components& ...>>iter() {
            constexpr auto compare_size{[](auto const* a, auto const* b) {
                return a->size() < b->size();
            }};

            auto& pivot{*std::min({&component_pool_base<Components>()...}, compare_size)};

            for (EntityId entity_id: pivot.entity_ids()) {
                if ((... && component_pool_base<Components, false>().has(entity_id))) {
                    co_yield {Entity{entity_id, *this}, component_pool<Components, false>().get(entity_id)...};
                }
            }
        }

        template <typename... Components, typename Lambda>
        void iter(Lambda lambda) {
            constexpr auto compare_size{[](auto const* a, auto const* b) {
                return a->size() < b->size();
            }};

            auto& pivot{*std::min({&component_pool_base<Components>()...}, compare_size)};

            for (EntityId entity_id: pivot.entity_ids()) {
                if ((... && component_pool_base<Components, false>().has(entity_id))) {
                    lambda(Entity{entity_id, *this}, component_pool<Components, false>().get(entity_id)...);
                }
            }
        }

        template <typename Component, bool check = true>
        Pool<Component>& component_pool() {
            constexpr ctti::detail::hash_t id{ctti::unnamed_type_id<Component>().hash()};

            if constexpr (check) {
                if (!m_pools.contains(id))
                    m_pools[id] = std::make_unique<Pool<Component>>();
            }

            return dynamic_cast<Pool<Component>&>(*m_pools[id]);
        }

    private:
        template <typename Component, bool check = true>
        IPool& component_pool_base() {
            constexpr ctti::detail::hash_t id{ctti::unnamed_type_id<Component>().hash()};

            if constexpr (check) {
                if (!m_pools.contains(id))
                    m_pools[id] = std::make_unique<Pool<Component>>();
            }

            return *m_pools[id];
        }

    private:
        EntityId m_current_entity_id{0};
        std::unordered_map<ctti::detail::hash_t, std::unique_ptr<IPool>> m_pools{};
    };
}


#endif  // RECS_WORLD_HPP
