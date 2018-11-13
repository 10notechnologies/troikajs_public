cmake_minimum_required(VERSION 2.8)
add_definitions("-std=c++11")

project(aicpp)

file(GLOB sources 
    ../module/troika_ai_emscripten.cpp
    ../module/troika_common.cpp
    ../module/troika_common.hpp
    ../module/troika_get_bid.cpp
    ../module/troika_get_pass_cards.cpp
    ../module/troika_get_play_cards.cpp
    ../module/troika_get_trump.cpp
)

add_executable(aicpp ${sources})
set_target_properties(aicpp PROPERTIES LINK_FLAGS "--bind -s DISABLE_EXCEPTION_CATCHING=0 --memory-init-file 0 -g3 -O0 -s WASM=0 -s ENVIRONMENT=web -o aicpp.js")



