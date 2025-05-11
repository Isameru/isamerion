#
#   MIT License
#   Copyright (c) 2025 Mariusz Łapiński
#
#     ▄█     ▄████████    ▄████████    ▄▄▄▄███▄▄▄▄      ▄████████    ▄████████  ▄█   ▄██████▄  ███▄▄▄▄
#     ███    ███    ███   ███    ███  ▄██▀▀▀███▀▀▀██▄   ███    ███   ███    ███ ███  ███    ███ ███▀▀▀██▄
#     ███▌   ███    █▀    ███    ███  ███   ███   ███   ███    █▀    ███    ███ ███▌ ███    ███ ███   ███
#     ███▌   ███          ███    ███  ███   ███   ███  ▄███▄▄▄      ▄███▄▄▄▄██▀ ███▌ ███    ███ ███   ███
#     ███▌ ▀███████████ ▀███████████  ███   ███   ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   ███▌ ███    ███ ███   ███
#     ███           ███   ███    ███  ███   ███   ███   ███    █▄  ▀███████████ ███  ███    ███ ███   ███
#     ███     ▄█    ███   ███    ███  ███   ███   ███   ███    ███   ███    ███ ███  ███    ███ ███   ███
#     █▀    ▄████████▀    ███    █▀    ▀█   ███   █▀    ██████████   ███    ███ █▀    ▀██████▀   ▀█   █▀
#                                                                   ███    ███
#

cmake_minimum_required(VERSION 3.20)
project(isamerion
    VERSION     1.0.0
    DESCRIPTION "Isamerion"
    LANGUAGES   CXX)

# ---―--―-――-―――-――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

# GLM: OpenGL Mathematics
# https://glm.g-truc.net/
#
include(FetchContent)
FetchContent_Declare(
	glm
	GIT_REPOSITORY  https://github.com/g-truc/glm.git
	GIT_TAG 	    1.0.1
)
FetchContent_MakeAvailable(glm)

# ---―--―-――-―――-――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

add_executable(isamerion
    src/core/clock.cpp
    src/gfx/display_window.cpp
    src/gfx/glbuffer.cpp
    src/gfx/glshader.cpp
    src/app.cpp
    src/nbody/galaxy_renderer.cpp
    src/nbody/galaxy_scene.cpp
    src/nbody/nbody_sim.cpp
    src/main.cpp
)

set_property(TARGET isamerion PROPERTY CXX_STANDARD 23)
target_include_directories(isamerion PUBLIC ${CMAKE_SOURCE_DIR}/src)

if(DEFINED EMSCRIPTEN)

    # WebAssembly using Emscripten toolchain
    #
    set_target_properties(isamerion PROPERTIES SUFFIX ".js")
    set(EM_LINK_FLAGS
        "--bind -s MODULARIZE -s EXPORT_NAME=\"Isamerion\""
        "--use-port=sdl2"
        "--use-port=sdl2_image"
        "--use-port=sdl2_ttf"
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
        "-sEXCEPTION_CATCHING_ALLOWED=[..]"
        "-sSDL2_IMAGE_FORMATS='[\"png\", \"jpg\"]'"
        "--embed-file ${CMAKE_SOURCE_DIR}/assets/KurintoMono-Rg.ttf@KurintoMono-Rg.ttf"
    )
    string(REPLACE ";" " " EM_LINK_FLAGS "${EM_LINK_FLAGS}")
    set_target_properties(isamerion PROPERTIES LINK_FLAGS ${EM_LINK_FLAGS})
    target_link_libraries(isamerion
        glm::glm-header-only
    )

elseif(DEFINED MSVC)

    # Windows for Visual Studio
    #
    include(${PROJECT_SOURCE_DIR}/cmake/deps-win.cmake)
    target_link_libraries(isamerion
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        GLEW
        glm::glm-header-only
        opengl32
    )

elseif(DEFINED UNIX)

    # GCC or clang on Linux
    #
    find_package(SDL2 REQUIRED)
    find_package(GLEW REQUIRED)
    target_include_directories(isamerion PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(isamerion
        SDL2
        SDL2_image
        SDL2_ttf
        GLEW::glew
        glm::glm-header-only
        GL
    )

else()

    message(FATAL_ERROR "Unsupported configuration: Tweak CMakeLists.txt manually.")

endif()

# ---―--―-――-―――-――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
