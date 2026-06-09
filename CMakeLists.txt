cmake_minimum_required(VERSION 3.15)
project(VoxSandbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WEB_BUILD "Build for web browser (Emscripten)" OFF)

# =========================
# Raylib
# =========================
if(WEB_BUILD)
    if(NOT EMSCRIPTEN)
        message(FATAL_ERROR
            "Emscripten not detected.\n"
            "Pass the toolchain file:\n"
            "  cmake -B build-web -DWEB_BUILD=ON \\\n"
            "    -DCMAKE_TOOLCHAIN_FILE=<path>/Emscripten.cmake")
    endif()

    set(PLATFORM Web CACHE STRING "Raylib platform" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.5
        GIT_SHALLOW    TRUE
    )
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)

else()
    # Native build — use homebrew raylib
    find_package(raylib REQUIRED)
endif()

# =========================
# Source files
# =========================
file(GLOB SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

# =========================
# Link raylib
# =========================
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# =========================
# Platform settings
# =========================
if(WEB_BUILD)
    target_link_options(${PROJECT_NAME} PRIVATE
        "SHELL:-s USE_GLFW=3"
        "SHELL:-s ASYNCIFY"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s TOTAL_STACK=64MB"
        "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/index.html"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/assets@assets"
    )
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreFoundation"
    )
endif()
