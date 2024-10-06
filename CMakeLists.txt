cmake_minimum_required(VERSION 3.29)
project(clion)

set(CMAKE_CXX_STANDARD 20)

add_executable(clion main.cpp)

if (WIN32)
    target_link_libraries(clion user32 gdi32)
    target_link_options(clion PRIVATE /subsystem:windows)
endif ()
