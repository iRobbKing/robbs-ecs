cmake_minimum_required(VERSION 3.22)
project(recs)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(deps/ctti)
add_subdirectory(deps/coro)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} INTERFACE ctti coro)
