cmake_minimum_required(VERSION 4.0)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)

project(bare_user32 C)

add_bare_module(bare_user32)

target_sources(
  ${bare_user32}
  PRIVATE
    binding.c
)

target_link_libraries(
  ${bare_user32}
  PUBLIC
    user32
)
