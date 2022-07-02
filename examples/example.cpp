#include <iostream>
#include <map>

#include <recs/world.hpp>


struct Position {
    float x{0}, y{0};
};


struct Velocity {
    float dx{0}, dy{0};
};


struct Jumpable {};
struct Jump {};


void update_jump_system(recs::World& world) {
    if (true) {  // *If pressed space.*
        for (auto [entity, _]: world.iter<Jumpable>()) {
            entity.get<Jump>();
        }
    }
}


void update_velocity_system(recs::World& world) {
    for (auto [entity, _]: world.iter<Jump>()) {
        entity.replace<Velocity>(0.f, 5.f);
    }
}


void update_position_system(recs::World& world) {
    for (auto [_, position, velocity]: world.iter<Position, Velocity>()) {
        position.x += velocity.dx;
        position.y += velocity.dy;
    }
}


void render_system(recs::World& world) {
    world.iter<Position>([](auto _, Position& position) {  // Another way to iterate over entities.
       std::cout << position.x << ' ' << position.y << '\n';
    });
}


struct Transform {};


template <>
struct recs::ComponentContainer<Transform> {   // There's a way to provide a custom container for components.
    using Type = std::map<int64_t, Transform>;   // For example for Transform it could be KD-Tree.
};


int main() {
    recs::World world{};

    for (int i{0}; i < 10'000'000; ++i) {
        auto entity{world.new_entity()};
        entity.get<Transform>();
    }

    auto entity1{world.new_entity()};
    entity1.get<Jumpable>();  // Entity::get creates component if it doesn't exist.
    entity1.get<Position>();  // If component isn't default constructable - assert error.

    entity1.replace<Position>(12.f, 0.f); // Entity::replace creates or replaces component.

    auto entity2{world.new_entity()};
    entity2.get<Jumpable>();
    entity2.get<Position>();
    entity2.get<Transform>();

    std::cout << entity1.has<Jumpable, Position>() << '\n';
    std::cout << entity2.has<Jumpable, Position>() << '\n';

    entity2.del<Jumpable>();

    std::cout << entity2.has<Jumpable>() << '\n';

    for (int i{0}; i < 5; ++i) {
        update_jump_system(world);
        update_velocity_system(world);
        world.clear<Jump>();  // We use Jump as an event, so we delete all instances before next iteration.
        update_position_system(world);
        world.clear<Velocity>();
        render_system(world);
        // Or world.clear<Jump, Velocity...>();
    }

    entity1.del();  // If no components are specified then all entity components are deleted.
    entity2.del();
}
