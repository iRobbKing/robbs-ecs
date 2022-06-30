cmake_minimum_required(VERSION 3.22)
project(recs_examples)

set(CMAKE_CXX_STANDARD 20)

option(RECS_BENCHMARK Off)

add_subdirectory(../ recs)

if (RECS_BENCHMARK)
    add_executable(${PROJECT_NAME} benchmark.cpp)
else()
    add_executable(${PROJECT_NAME} example.cpp)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE recs)
