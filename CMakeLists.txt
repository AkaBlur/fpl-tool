cmake_minimum_required(VERSION 3.1)

project(
    foobarPlaylistTool
    VERSION 0.0
    LANGUAGES CXX
)

set(PL_SOURCES
    "foobarPlaylistTool.cpp"
)

add_executable(
    foobarPlaylistTool
    ${PL_SOURCES}
)

target_compile_features(
    foobarPlaylistTool
    PUBLIC
    cxx_std_17
)