cmake_minimum_required(VERSION 3.10.0) 
project(SkyLands LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)


# --- GLFW Setup ---
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw)

# --- GLM Setup ---
add_subdirectory(libs/glm)

# --- Vulkan Setup ---
find_package(Vulkan REQUIRED)

# macOS Specific: Ensure MoltenVK is found if you aren't using the dynamic loader
if(APPLE)
    # The Vulkan SDK on macOS often needs a hint to find MoltenVK
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,${Vulkan_LIBRARY_DIR}")
endif()

# --- Sources ---
file(GLOB SKYLANDS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SKYLANDS_SOURCES})

# Include your project headers
target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# --- Linking ---
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        Vulkan::Vulkan
        glm::glm
)

# --- Compile Definitions ---
target_compile_definitions(${PROJECT_NAME} PRIVATE
    # Required for Vulkan-Hpp dynamic dispatching
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    # Enables C++20 designated initializers: vk::InstanceCreateInfo info{.sType = ...}
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

# macOS Specific: Force the app to look for MoltenVK in the SDK path
if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VK_USE_PLATFORM_MACOS_MVK)
endif()
