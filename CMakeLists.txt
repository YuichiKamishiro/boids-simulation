cmake_minimum_required(VERSION 3.30)

project(test)

add_executable(test main.cpp)
target_link_libraries(test sfml-window sfml-graphics sfml-window sfml-system)