#ifndef RECS_COMPONENT_POOL_HPP
#define RECS_COMPONENT_POOL_HPP

#include <cassert>
#include <type_traits>

#include <coro/unique_generator.h>

#include "detail.hpp"
#include "component_container.hpp"


namespace recs {
    struct IPool {
        virtual bool has(EntityId entity_id) const = 0;
        virtual void del(EntityId entity_id) = 0;
        virtual void clear() = 0;
        virtual size_t size() const = 0;
        virtual unique_generator<EntityId> entity_ids() = 0;

        virtual ~IPool() = default;
    };


    template <typename Component>
    struct Pool : public IPool {
        Component& get(EntityId entity_id) {
            if constexpr (std::is_default_constructible_v<Component>) {
                return m_components[entity_id];
            } else {
                auto it{m_components.find(entity_id)};

                if (it == m_components.end())
                    assert(false && "Component must be added before being accessed.");

                return it->second;
            }
        }

        template <typename... Ts>
        Component& replace(EntityId entity_id, Ts&& ... params) {
            return m_components.insert_or_assign(entity_id, Component{std::forward<Ts>(params)...}).first->second;
        }

        Component& replace(EntityId entity_id, Component const& component) {
            return m_components.insert_or_assign(entity_id, component).first->second;
        }

        Component& replace(EntityId entity_id, Component&& component) {
            return m_components.insert_or_assign(entity_id, std::move(component)).first->second;
        }

        bool has(EntityId entity_id) const override {
            return m_components.contains(entity_id);
        }

        void del(EntityId entity_id) override {
            m_components.erase(entity_id);
        }

        void clear() override {
            m_components.clear();
        }

        size_t size() const override {
            return m_components.size();
        }

        unique_generator<EntityId> entity_ids() override {
            for (auto& [entity_id, _]: m_components) {
                co_yield entity_id;
            }
        }

        auto& container() {
            return m_components;
        }

    private:
        typename ComponentContainer<Component>::Type m_components;
    };
}


#endif // RECS_COMPONENT_POOL_HPP
