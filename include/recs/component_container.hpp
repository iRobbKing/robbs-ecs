#ifndef RECS_COMPONENT_CONTAINER_HPP
#define RECS_COMPONENT_CONTAINER_HPP

#include <cstdint>
#include <unordered_map>

#include "detail.hpp"


namespace recs {
    template <typename Component>
    struct ComponentContainer {
        using Type = std::unordered_map<EntityId, Component>;
    };
}


#endif // RECS_COMPONENT_CONTAINER_HPP
