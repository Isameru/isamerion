
include(${PROJECT_SOURCE_DIR}/deps-win/SDL2-2.30.7/cmake/sdl2-config.cmake)
include(${PROJECT_SOURCE_DIR}/deps-win/SDL2_image-2.8.2/cmake/sdl2_image-config.cmake)
include(${PROJECT_SOURCE_DIR}/deps-win/SDL2_ttf-2.22.0/cmake/sdl2_ttf-config.cmake)

set(_glew_dir ${PROJECT_SOURCE_DIR}/deps-win/glew-2.2.0)

add_library(GLEW SHARED IMPORTED)
set_target_properties(GLEW
    PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_glew_dir}/include"
        IMPORTED_IMPLIB "${_glew_dir}/lib/Release/x64/glew32.lib"
        IMPORTED_LOCATION "${_glew_dir}/bin/x64/glew32.dll"
)
