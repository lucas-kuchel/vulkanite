cmake_minimum_required(VERSION 3.21)

include(FetchContent)

project(basic-test LANGUAGES CXX)

file(GLOB_RECURSE SOURCES "source/*.cpp")

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

add_executable(basic-test ${SOURCES})

target_include_directories(basic-test PRIVATE
    ${VulkanMemoryAllocator_SOURCE_DIR}/include
    ${glfw_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    "../../"
)

target_link_libraries(basic-test PRIVATE
    glfw
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
)
