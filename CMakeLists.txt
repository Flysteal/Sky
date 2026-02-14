cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)

project(Sky VERSION 0.1.0 LANGUAGES CXX C)

# Include FetchContent module
include(FetchContent)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw3 QUIET)

if (NOT glfw3_FOUND)
    message(STATUS "GLFW not found, fetching...")

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG latest
    )

    FetchContent_MakeAvailable(glfw)
else()
    message(STATUS "GLFW found!")
endif()

add_executable(SkyLands)

file(GLOB SRC "${CMAKE_SOURCE_DIR}/App/*.cpp"
              "${CMAKE_SOURCE_DIR}/main.cpp"
              "${CMAKE_SOURCE_DIR}/Vulkan/*.cpp"
              "${CMAKE_SOURCE_DIR}/Visual/*.cpp")
target_sources(SkyLands PRIVATE ${SRC})

target_include_directories(SkyLands PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(SkyLands PRIVATE
    glfw
    glm::glm
    Vulkan::Vulkan
)
