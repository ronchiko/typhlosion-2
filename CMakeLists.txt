cmake_minimum_required(VERSION 3.20)

project(Typhlosion)

file(GLOB Source CONFIGURE_DEPENDS *.cpp)
add_executable(Typhlosion ${Source})

target_include_directories(Typhlosion PRIVATE headers)