#ifndef RECS_ENTITY_HPP
#define RECS_ENTITY_HPP


namespace recs {
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
}


#endif // RECS_ENTITY_HPP
