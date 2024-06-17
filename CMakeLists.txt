cmake_minimum_required(VERSION 3.13)

# internal debug switch
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FPL_DEBUG 1)
    
else()
    set(FPL_DEBUG 0)

endif()

set(FPL_VERSION 0.2)
set(BUILD_DATE "")
string(TIMESTAMP BUILD_DATE "%Y-%m-%d, %H:%M UTC" UTC)

if (MSVC)
    set(CMAKE_GENERATOR_PLATFORM x64)
endif()

project(
    fplTool
    VERSION ${FPL_VERSION}
    LANGUAGES CXX
)

add_subdirectory("fplTool")
