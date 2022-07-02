#include <chrono>
#include <iostream>

#include <recs/world.hpp>


struct Some {};
struct Another {};


void benchmark_creation(recs::World& world, int n) {
    auto start{std::chrono::high_resolution_clock::now()};

    for (int i {0}; i < n; ++i) {
        auto entity{world.new_entity()};
        entity.get<Some>();
        entity.get<Another>();
    }

    auto end{std::chrono::high_resolution_clock::now()};

    std::cout << "Created entities with 2 components in: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
}


void benchmark_modification(recs::World& world) {
    auto start{std::chrono::high_resolution_clock::now()};

    for (auto [entity, some, another]: world.iter<Some, Another>()) {
        entity.replace<Some>();
        entity.replace<Another>();
    }

    auto end{std::chrono::high_resolution_clock::now()};

    std::cout << "Modified 2 components in entities in: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
}


void benchmark_iteration(recs::World& world) {
    auto start{std::chrono::high_resolution_clock::now()};

    for (auto [entity, some, another]: world.iter<Some, Another>()) {

    }

    auto end{std::chrono::high_resolution_clock::now()};

    std::cout << "Iterated over entities with 2 components in: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
}


void benchmark_cleanup(recs::World& world) {
    auto start{std::chrono::high_resolution_clock::now()};

    world.clear<Some, Another>();

    auto end{std::chrono::high_resolution_clock::now()};

    std::cout << "Cleared 2 components with instances each in: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
}


int main() {
    recs::World world{};

    benchmark_creation(world, N_ENTITIES);
    benchmark_modification(world);
    benchmark_iteration(world);
    benchmark_cleanup(world);
}
