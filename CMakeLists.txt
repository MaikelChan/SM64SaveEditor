
include(FetchContent)

# Used features that require certain minimum version:
# * 3.11: Required for setting GLVND OpenGL preference.
# * 3.15: Removes the '/W3' MSVC warning flag added by CMake by default, so another warning level can be set with 'target_compile_options' without causing warnings.
cmake_minimum_required (VERSION 3.19)

# SDL3 --------------------------------------------------------------------------------------------

set(LINK_SDL3_STATICALLY TRUE)

set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/out/)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.4
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

if (LINK_SDL3_STATICALLY)
    set(SDL_SHARED OFF)
    set(SDL_STATIC ON)
else()
    set(SDL_SHARED ON)
    set(SDL_STATIC OFF)
endif()

# Subsystems

set(SDL_AUDIO_DEFAULT OFF)
set(SDL_VIDEO_DEFAULT ON)
set(SDL_GPU_DEFAULT ON)
set(SDL_RENDER_DEFAULT OFF)
set(SDL_CAMERA_DEFAULT OFF)
set(SDL_JOYSTICK_DEFAULT OFF)
set(SDL_HAPTIC_DEFAULT OFF)
set(SDL_HIDAPI_DEFAULT OFF)
set(SDL_POWER_DEFAULT OFF)
set(SDL_SENSOR_DEFAULT OFF)
set(SDL_DIALOG_DEFAULT ON)

# GPU Drivers

set(SDL_OFFSCREEN OFF)
set(SDL_OPENGL OFF)
set(SDL_OPENGLES OFF)
set(SDL_DUMMYVIDEO OFF)
set(SDL_DIRECTX OFF)
set(SDL_VULKAN ON)

# Trying to disable D3D12 in SDL GPU, but it doesn't
# seem work unless modifying SDL's CMakeLists.txt.

#set(SDL_GPU_D3D12 0)
#unset(SDL_GPU_D3D12)

# Disable Wayland on Linux. Seems that there are still problems,
# and this way we reduce the size of the executable a bit.

set(SDL_WAYLAND OFF)
set(SDL_WAYLAND_LIBDECOR OFF)
set(SDL_WAYLAND_LIBDECOR_SHARED OFF)
set(SDL_WAYLAND_SHARED OFF)

# Other projects

set(SDL_TEST_LIBRARY OFF)
set(SDL_TESTS OFF)
set(SDL_DISABLE_INSTALL OFF)
set(SDL_DISABLE_INSTALL_DOCS OFF)
set(SDL_INSTALL_TESTS OFF)
set(SDL_EXAMPLES OFF)

FetchContent_MakeAvailable(SDL3)

# Main project ------------------------------------------------------------------------------------

project ("SM64SaveEditor" VERSION "1.3.1")
add_subdirectory ("src")
