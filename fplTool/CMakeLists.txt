set(PL_SOURCES
    "fplTool.cpp"

    "CommandQueue.cpp"
    "FoobarPlaylist.cpp"
    "MediaEntry.cpp"
)

set(PL_HEADERS
    "CommandQueue.h"
    "FoobarPlaylist.h"
    "MediaEntry.h"
    
    "Log.h"
    "Util.h"
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

if (NOT MSVC)
    target_compile_options(
        fplTool
        PUBLIC
        -m64
    )
endif()

target_compile_features(
    fplTool
    PUBLIC
    cxx_std_20
)

target_compile_definitions(
    fplTool
    PUBLIC
    FPL_DEBUG=${FPL_DEBUG}
    FPL_VERSION="${FPL_VERSION}"
    BUILD_DATE="${BUILD_DATE}"
    FPL_BUILD_PLATFORM="non-Windows"
)

if (WIN32)
    target_compile_definitions(
        fplTool
        PUBLIC
        PLATFORM_WIN=1
        FPL_BUILD_PLATFORM="Windows"
    )
endif()

install(TARGETS fplTool DESTINATION bin)