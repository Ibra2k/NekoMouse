cmake_minimum_required(VERSION 3.16)
project(cat LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(raylib REQUIRED)
find_path(ASIO_INCLUDE_DIR asio.hpp)


if (APPLE)
    set(PLATFORM_SOURCES src/client/platform/macos_mouse.mm)
elseif(UNIX)
    set(PLATFORM_SOURCES src/client/platform/linux_mouse.cpp)
    find_package(X11 REQUIRED)
endif()

add_executable(kitty

    src/client/main.cpp

    src/common/protocol.cpp

    src/client/net/network_client.cpp
    
    src/client/render/animation.cpp
    src/client/render/state.cpp
    src/client/render/cat_renderer.cpp

    src/client/settings/settings.cpp

    src/client/ui/menu.cpp
    
    src/client/world/cat.cpp
    src/client/world/remote_cats.cpp

    ${PLATFORM_SOURCES}
)

add_executable(kitty_server

    src/server/main.cpp
    src/server/server.cpp
    src/server/session.cpp

    src/common/protocol.cpp

)

target_include_directories(kitty PRIVATE
    include
    common
    ${ASIO_INCLUDE_DIR}
    /opt/homebrew/include   # Apple Silicon
    /usr/local/include      # Intel
)


target_link_libraries(kitty PRIVATE raylib)
if (APPLE)
    target_link_libraries(kitty PRIVATE "-framework Cocoa")
elseif(UNIX)
    target_link_libraries(kitty PRIVATE X11::X11)
endif()

# Copy the assets folder next to the built executable so GetApplicationDirectory()
# based paths (e.g. "assets/white_cat/up1.png") resolve correctly at runtime,
# whether run directly from build/ or from inside an .app bundle.
add_custom_command(TARGET kitty POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:kitty>/assets
)
