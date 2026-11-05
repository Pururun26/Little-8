cmake_minimum_required(VERSION 3.16)
project(Little8 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ---------- raylib (prebuilt mingw-w64) ----------
set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/raylib/raylib-6.0_win64_mingw-w64")

if(NOT EXISTS "${RAYLIB_DIR}/include/raylib.h")
    message(FATAL_ERROR "raylib.h not found at ${RAYLIB_DIR}/include/raylib.h")
endif()

find_file(RAYLIB_LIB NAMES libraylib.a
    PATHS "${RAYLIB_DIR}/lib" NO_DEFAULT_PATH)

if(NOT RAYLIB_LIB)
    message(FATAL_ERROR "libraylib.a not found at ${RAYLIB_DIR}/lib")
endif()

message(STATUS "raylib: ${RAYLIB_LIB}")

add_library(raylib STATIC IMPORTED)
set_target_properties(raylib PROPERTIES
    IMPORTED_LOCATION "${RAYLIB_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${RAYLIB_DIR}/include")

# ---------- Little-8 engine (DLL) ----------
add_library(little8 SHARED src/little8_api.c)
set_target_properties(little8 PROPERTIES PREFIX "")
target_include_directories(little8 PRIVATE src)
target_compile_definitions(little8 PRIVATE LITTLE8_BUILD)
target_link_libraries(little8 PRIVATE raylib)

if(WIN32)
    target_link_libraries(little8 PRIVATE opengl32 gdi32 winmm shell32)
endif()

# ---------- Example game (EXE) ----------
add_executable(game src/main.c)
target_include_directories(game PRIVATE src)
target_link_libraries(game PRIVATE little8)

# Кладём little8.dll рядом с game.exe, чтобы Windows её нашёл
if(WIN32)
    add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:little8>" "$<TARGET_FILE_DIR:game>")
endif()