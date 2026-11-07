cmake_minimum_required(VERSION 3.16)
project(Little8 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ---------- raylib (prebuilt) ----------
if(WIN32)
    set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/raylib/raylib-6.0_win64_mingw-w64")
    set(RAYLIB_LIB_NAMES libraylib.a)
    set(RAYLIB_SYSTEM_LIBS opengl32 gdi32 winmm shell32)
elseif(APPLE)
    message(FATAL_ERROR "macOS prebuilt not bundled; build raylib from source")
else()
    # Linux: amd64 или arm64
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/raylib/raylib-6.0_linux_arm64")
    else()
        set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/raylib/raylib-6.0_linux_amd64")
    endif()
    set(RAYLIB_LIB_NAMES libraylib.a libraylib.so)
    set(RAYLIB_SYSTEM_LIBS GL m pthread dl rt X11)
endif()

if(NOT EXISTS "${RAYLIB_DIR}/include/raylib.h")
    message(FATAL_ERROR "raylib.h not found at ${RAYLIB_DIR}/include/raylib.h")
endif()

# ищем libraylib.a (приоритет) или libraylib.so
set(RAYLIB_LIB "")
foreach(name IN LISTS RAYLIB_LIB_NAMES)
    find_file(_found NAMES ${name} PATHS "${RAYLIB_DIR}/lib" NO_DEFAULT_PATH)
    if(_found)
        set(RAYLIB_LIB "${_found}")
        break()
    endif()
endforeach()

if(NOT RAYLIB_LIB)
    message(FATAL_ERROR "raylib lib not found at ${RAYLIB_DIR}/lib "
                        "(tried: ${RAYLIB_LIB_NAMES})")
endif()

message(STATUS "raylib: ${RAYLIB_LIB}")

add_library(raylib STATIC IMPORTED)
set_target_properties(raylib PROPERTIES
    IMPORTED_LOCATION "${RAYLIB_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${RAYLIB_DIR}/include")

# ---------- Little-8 (shared library) ----------
add_library(little8 SHARED
    src/little8_api.c
    src/little8_audio.c
    src/little8_save.c
)

set_target_properties(little8 PROPERTIES
    PREFIX ""
    OUTPUT_NAME "little8"
)

target_include_directories(little8 PRIVATE src)
target_compile_definitions(little8 PRIVATE LITTLE8_BUILD)
target_link_libraries(little8 PRIVATE raylib ${RAYLIB_SYSTEM_LIBS})

# ---------- size optimization ----------
if(NOT MSVC)
    target_compile_options(little8 PRIVATE
        $<$<CONFIG:Release>:-O2>
        $<$<CONFIG:Release>:-fno-asynchronous-unwind-tables>
        $<$<CONFIG:Release>:-fno-unwind-tables>
        -ffunction-sections
        -fdata-sections
    )

    target_link_options(little8 PRIVATE
        $<$<CONFIG:Release>:-Wl,--gc-sections>
        $<$<CONFIG:Release>:-s>
    )
endif()