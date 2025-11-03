cmake_minimum_required(VERSION 3.21)

project(vulkanite-engine LANGUAGES CXX)

include(FetchContent)

find_package(Vulkan REQUIRED)

FetchContent_Declare(
    VulkanMemoryAllocator
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG master
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(VulkanMemoryAllocator)

add_library(vulkanite INTERFACE)

target_include_directories(vulkanite INTERFACE
    ${VulkanMemoryAllocator_SOURCE_DIR}/include
    ${glfw_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
)

target_link_libraries(vulkanite INTERFACE
    glfw
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND MSVC)
    target_compile_definitions(vulkanite INTERFACE VULKANITE_COMPILER_CLANG_CL)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_definitions(vulkanite INTERFACE VULKANITE_COMPILER_CLANG)
endif()

if(MSVC)
    target_compile_definitions(vulkanite INTERFACE VULKANITE_COMPILER_MSVC)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GCC")
    target_compile_definitions(vulkanite INTERFACE VULKANITE_COMPILER_GCC)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(vulkanite INTERFACE VULKANITE_BUILD_TYPE_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(vulkanite INTERFACE VULKANITE_BUILD_TYPE_RELEASE)
endif()

if(APPLE)
    target_compile_definitions(vulkanite INTERFACE VULKANITE_PLATFORM_APPLE)
endif()

if(UNIX)
    target_compile_definitions(vulkanite INTERFACE VULKANITE_PLATFORM_UNIX)
elseif(WIN32)
    target_compile_definitions(vulkanite INTERFACE VULKANITE_PLATFORM_WIN32)
endif()
