cmake_minimum_required(VERSION 3.10)
project(vkTest)

set(CMAKE_CXX_STANDARD 23)

find_package (Vulkan REQUIRED)

file(GLOB SOURCE_FILES src/*.cpp)

add_executable(${PROJECT_NAME} main.cpp ${SOURCE_FILES})

target_link_libraries (${PROJECT_NAME} Vulkan::Vulkan)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ../
)

