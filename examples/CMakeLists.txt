cmake_minimum_required(VERSION 3.22)
project(recs_examples)

set(CMAKE_CXX_STANDARD 20)

option(RECS_BENCHMARK OFF)
set(N_ENTITIES 10'000'000)

add_subdirectory(../ recs)

if (RECS_BENCHMARK)
    add_executable(${PROJECT_NAME} example.cpp)
else()
    add_executable(${PROJECT_NAME} benchmark.cpp)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE N_ENTITIES=${N_ENTITIES})
target_link_libraries(${PROJECT_NAME} PRIVATE recs)
