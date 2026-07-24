cmake_minimum_required(VERSION 3.16)
project(cat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

if (APPLE)
    set(PLATFORM_SOURCES src/macos_mouse.mm)
elseif(UNIX)
    set(PLATFORM_SOURCES src/linux_mouse.cpp)
    find_package(X11 REQUIRED)
endif()


add_executable(kitty
    src/main.cpp
    src/cat.cpp
    src/animation.cpp
    src/state.cpp
    ${PLATFORM_SOURCES}
)

target_include_directories(kitty PRIVATE include)
target_link_libraries(kitty PRIVATE raylib)

if (APPLE)
    target_link_libraries(kitty PRIVATE "-framework Cocoa")
elseif(UNIX)
    target_link_libraries(kitty PRIVATE X11::X11)
endif()
