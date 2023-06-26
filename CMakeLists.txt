cmake_minimum_required(VERSION 3.25)
project(SDL_playground C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SOURCES "simulation/*.c")

include_directories(simulation assets)

add_executable(SDL_playground main.c ${SOURCES})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

target_link_libraries(SDL_playground
                      PRIVATE
                      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
                      $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
                      )
target_link_libraries(SDL_playground
                      PRIVATE
                      $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
                      )