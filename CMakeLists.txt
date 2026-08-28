cmake_minimum_required(VERSION 3.24)
project(ui_framework_demo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# This demo intentionally pins the minimum SDL3 baseline we want to validate.
set(UI_FRAMEWORK_SDL_VERSION "3.2.0")
set(UI_FRAMEWORK_SDL_GIT_TAG "release-3.2.0")
set(UI_FRAMEWORK_SDL_IMAGE_GIT_TAG "release-3.2.0")
set(UI_FRAMEWORK_SDL_TTF_GIT_TAG "release-3.2.0")

# Build SDL and its addons with one consistent shared-linking policy.
set(SDL_SHARED ON CACHE BOOL "Build SDL3 shared library" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Build SDL3 static library" FORCE)
set(SDL_TESTS OFF CACHE BOOL "Build SDL3 tests" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "Build SDL3 examples" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "Install SDL3" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG ${UI_FRAMEWORK_SDL_GIT_TAG}
    GIT_SHALLOW TRUE
    OVERRIDE_FIND_PACKAGE
)

# Keep optional SDL_image features small for the smoke test.
set(SDLIMAGE_SAMPLES OFF CACHE BOOL "Build SDL3_image samples" FORCE)
set(SDLIMAGE_TESTS OFF CACHE BOOL "Build SDL3_image tests" FORCE)
set(SDLIMAGE_INSTALL OFF CACHE BOOL "Install SDL3_image" FORCE)
set(SDLIMAGE_PNG OFF CACHE BOOL "Enable PNG support" FORCE)
set(SDLIMAGE_JPG OFF CACHE BOOL "Enable JPEG support" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "Enable TIFF support" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "Enable WebP support" FORCE)
set(SDLIMAGE_AVIF OFF CACHE BOOL "Enable AVIF support" FORCE)
set(SDLIMAGE_JXL OFF CACHE BOOL "Enable JPEG XL support" FORCE)
set(SDLIMAGE_SVG OFF CACHE BOOL "Enable SVG support" FORCE)

FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG ${UI_FRAMEWORK_SDL_IMAGE_GIT_TAG}
    GIT_SHALLOW TRUE
    OVERRIDE_FIND_PACKAGE
)

# SDL_ttf uses vendored third-party dependencies so the compatibility test
# does not depend on system FreeType/HarfBuzz installations.
set(SDLTTF_VENDORED ON CACHE BOOL "Use vendored SDL_ttf dependencies" FORCE)
set(SDLTTF_SAMPLES OFF CACHE BOOL "Build SDL3_ttf samples" FORCE)
set(SDLTTF_TESTS OFF CACHE BOOL "Build SDL3_ttf tests" FORCE)
set(SDLTTF_INSTALL OFF CACHE BOOL "Install SDL3_ttf" FORCE)

FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG ${UI_FRAMEWORK_SDL_TTF_GIT_TAG}
    GIT_SHALLOW TRUE
    OVERRIDE_FIND_PACKAGE
)

# Fetch the framework under test from its standalone repository.
FetchContent_Declare(
    ui_framework
    GIT_REPOSITORY https://github.com/Valeri-afk/ui_framework.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3 SDL3_image SDL3_ttf ui_framework)

add_executable(ui_framework_demo
    src/main.cpp
)

target_link_libraries(ui_framework_demo
    PRIVATE
        ui_framework::ui_framework
)

# Keep the runtime DLLs next to the demo executable so the smoke test can be
# launched directly from the build tree on Windows. ui_framework itself is a
# static library; its SDL dependencies are the runtime DLLs we need here.
if(WIN32)
    add_custom_command(
        TARGET ui_framework_demo
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:ui_framework_demo>
            $<TARGET_FILE_DIR:ui_framework_demo>
        COMMAND_EXPAND_LISTS
    )
endif()

message(STATUS "ui_framework SDL3 compatibility test: ${UI_FRAMEWORK_SDL_VERSION}")
message(STATUS "SDL3 source: ${SDL3_SOURCE_DIR}")
message(STATUS "SDL3_image source: ${SDL3_image_SOURCE_DIR}")
message(STATUS "SDL3_ttf source: ${SDL3_ttf_SOURCE_DIR}")
message(STATUS "ui_framework source: ${ui_framework_SOURCE_DIR}")
