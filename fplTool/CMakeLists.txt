set(PL_SOURCES
    "fplTool.cpp"

    "CommandQueue.cpp"
    "FoobarPlaylist.cpp"
)

set(PL_HEADERS
    "CommandQueue.h"
    "FoobarPlaylist.h"
    "Log.h"
)

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${PL_SOURCES})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${PL_HEADERS})

add_executable(
    fplTool
    ${PL_SOURCES}
    ${PL_HEADERS}
)

target_include_directories(
    fplTool
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_features(
    fplTool
    PUBLIC
    cxx_std_17
)

target_compile_definitions(
    fplTool
    PUBLIC
    DEBUG=${BUILD_DEBUG}
    FPL_VERSION="${FPL_VERSION}"
    BUILD_DATE="${BUILD_DATE}"
)