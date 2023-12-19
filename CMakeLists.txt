cmake_minimum_required(VERSION 3.13)

option(BUILD_DEBUG "Build in debug mode" 0)

set(FPL_VERSION 0.2)
set(BUILD_DATE "")
string(TIMESTAMP BUILD_DATE "%Y-%m-%d, %H:%M UTC" UTC)

project(
    fplTool
    VERSION ${FPL_VERSION}
    LANGUAGES CXX
)

add_subdirectory("fplTool")