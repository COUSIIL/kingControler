cmake_minimum_required(VERSION 3.10)
project(sfmldemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

add_executable(sfmldemo main.cpp)
target_link_libraries(sfmldemo PRIVATE sfml-system sfml-window sfml-graphics)
