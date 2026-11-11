cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(cat LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# ---------------------------------------------------------
# 1. Raylib (fetched from GitHub)
#    The client uses all of it; the server only needs raymath.h.
# ---------------------------------------------------------
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        raylib
        GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
        GIT_TAG "5.5"
)
FetchContent_MakeAvailable(raylib)

# ---------------------------------------------------------
# 2. Asio (standalone, header-only)
#    Use a system install if there is one, otherwise download it.
# ---------------------------------------------------------
find_path(ASIO_INCLUDE_DIR asio.hpp)

if (NOT ASIO_INCLUDE_DIR)
    FetchContent_Declare(
            asio
            GIT_REPOSITORY "https://github.com/chriskohlhoff/asio.git"
            GIT_TAG "asio-1-30-2"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(asio)
    set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include CACHE PATH "" FORCE)
endif()

add_library(asio_headers INTERFACE)
target_include_directories(asio_headers INTERFACE ${ASIO_INCLUDE_DIR})
target_compile_definitions(asio_headers INTERFACE ASIO_STANDALONE)

if (WIN32)
    # Asio on Windows needs Winsock and a target Windows version (0x0A00 = Windows 10).
    target_compile_definitions(asio_headers INTERFACE _WIN32_WINNT=0x0A00 WIN32_LEAN_AND_MEAN)
    target_link_libraries(asio_headers INTERFACE ws2_32 mswsock)

    # The shared code includes a few Linux-only headers. On Windows we generate
    # stand-ins so the server builds without touching the sources:
    #   arpa/inet.h, netinet/in.h -> Winsock (provides htonl/ntohl/htons/ntohs)
    #   X11/Xlib.h                -> empty (the server doesn't use X11)
    # Windows headers also #define near/far, which breaks raymath.h, so undo them.
    set(WIN_COMPAT_DIR ${CMAKE_BINARY_DIR}/win_compat)
    set(WINSOCK_SHIM "#pragma once\n#include <winsock2.h>\n#include <ws2tcpip.h>\n#undef near\n#undef far\n")
    file(WRITE ${WIN_COMPAT_DIR}/arpa/inet.h  "${WINSOCK_SHIM}")
    file(WRITE ${WIN_COMPAT_DIR}/netinet/in.h "${WINSOCK_SHIM}")
    file(WRITE ${WIN_COMPAT_DIR}/X11/Xlib.h   "#pragma once\n")
    target_include_directories(asio_headers INTERFACE ${WIN_COMPAT_DIR})
endif()

# ---------------------------------------------------------
# 3. Server (builds on Windows, macOS and Linux)
# ---------------------------------------------------------
add_executable(kitty_server
    src/server/main.cpp
    src/server/server.cpp
    src/server/session.cpp

    src/common/protocol.cpp
)

target_link_libraries(kitty_server PRIVATE asio_headers raylib)
if (WIN32)
    # protocol.cpp uses memcpy/assert/std::array without including them directly.
    target_compile_options(kitty_server PRIVATE "SHELL:-include cstring" "SHELL:-include cassert" "SHELL:-include array")
endif()

# ---------------------------------------------------------
# 4. Client (needs a platform mouse backend)
# ---------------------------------------------------------
if (WIN32)
    # Built as its own library: it needs the full <windows.h>, while the rest of
    # the client hides parts of it (see NOGDI/NOUSER below).
    add_library(kitty_platform STATIC src/client/platform/windows_mouse.cpp)
    target_link_libraries(kitty_platform PRIVATE user32)
    set(PLATFORM_SOURCES "")
    set(HAVE_MOUSE_BACKEND ON)
elseif (APPLE)
    set(PLATFORM_SOURCES src/client/platform/macos_mouse.mm)
    set(HAVE_MOUSE_BACKEND ON)
elseif (UNIX)
    set(PLATFORM_SOURCES src/client/platform/linux_mouse.cpp)
    set(HAVE_MOUSE_BACKEND ON)
    find_package(X11 REQUIRED)
endif()

if (HAVE_MOUSE_BACKEND)
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

    target_include_directories(kitty PRIVATE
        include
        common
        /opt/homebrew/include   # Apple Silicon
        /usr/local/include      # Intel
    )

    target_link_libraries(kitty PRIVATE raylib asio_headers)
    if (WIN32)
        target_link_libraries(kitty PRIVATE kitty_platform)
        # raylib and <windows.h> (pulled in by Asio) both define Rectangle,
        # CloseWindow, ShowCursor, DrawText, LoadImage... Hide the GDI/USER parts
        # of <windows.h> so raylib's versions win.
        target_compile_definitions(kitty PRIVATE NOGDI NOUSER)
        target_compile_options(kitty PRIVATE "SHELL:-include cstring" "SHELL:-include cassert" "SHELL:-include array")
    elseif (APPLE)
        target_link_libraries(kitty PRIVATE "-framework Cocoa")
    else()
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
else()
    message(STATUS "No mouse backend for this platform: building kitty_server only.")
endif()
