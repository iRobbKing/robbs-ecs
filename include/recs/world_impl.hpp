#ifndef RECS_WORLD_IMPL_HPP
#define RECS_WORLD_IMPL_HPP


namespace recs {
    Entity World::new_entity() {
        return {m_current_entity_id++, *this};
    }

    template <typename... Components>
    unique_generator<std::tuple<Entity, Components& ...>> World::iter() {
        constexpr auto compare_size{[](auto const* a, auto const* b) {
            return a->size() < b->size();
        }};

        auto& pivot{*std::min({&get_component_pool_base<Components>()...}, compare_size)};

        for (EntityId entity_id: pivot.entity_ids()) {
            if ((... && get_component_pool_base<Components, false>().has(entity_id))) {
                co_yield {Entity{entity_id, *this}, get_component_pool<Components, false>().get(entity_id)...};
            }
        }
    }

    template <typename... Components, typename Lambda>
    void World::iter(Lambda lambda) {
        constexpr auto compare_size{[](auto const* a, auto const* b) {
            return a->size() < b->size();
        }};

        auto& pivot{*std::min({&get_component_pool_base<Components>()...}, compare_size)};

        for (EntityId entity_id: pivot.entity_ids()) {
            if ((... && get_component_pool_base<Components, false>().has(entity_id))) {
                lambda(Entity{entity_id, *this}, get_component_pool<Components, false>().get(entity_id)...);
            }
        }
    }
}


#endif // RECS_WORLD_IMPL_HPP
