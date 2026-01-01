cmake_minimum_required(VERSION 3.10)
project(vkTest)

set(CMAKE_CXX_STANDARD 26)

if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
	message(DETECTED MINGWWWW)
	set(QT6_DIR "C:/Qt/6.10.1/mingw_64")
else()
	set(QT6_DIR "C:/Qt/6.10.1/msvc2022_64/lib/cmake")
endif()
set(CMAKE_PREFIX_PATH ${QT6_DIR})

find_package (Qt6 REQUIRED COMPONENTS Core Gui Widgets)

if (UNIX)
    find_package (Vulkan REQUIRED dxc)
	#find_package(Slang CONFIG HINTS "$ENV{VULKAN_SDK}/lib/cmake")
else()
    find_package (Vulkan REQUIRED)
endif()

file(GLOB SOURCE_FILES native/*.cpp beamcast/*.cpp)

add_subdirectory( lib/glm )

add_executable(${PROJECT_NAME} main.cpp ${SOURCE_FILES})

add_definitions(${Qt6Core_DEFINITIONS})
target_link_libraries (${PROJECT_NAME} Vulkan::Vulkan)
if (WIN32)
	target_link_libraries(${PROJECT_NAME} $ENV{VULKAN_SDK}/Lib/slangd.lib)
else()
	target_link_libraries(${PROJECT_NAME} $ENV{VULKAN_SDK}/lib/libslang.so)
endif()

if (UNIX)
    target_link_libraries(${PROJECT_NAME} Vulkan::dxc_lib)
endif()
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Gui Qt6::Widgets)
target_include_directories(${PROJECT_NAME} PRIVATE ${Qt6Gui_PRIVATE_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE lib/glm)
target_include_directories(${PROJECT_NAME} PRIVATE lib/sdp_2023)

target_compile_options(${PROJECT_NAME} PUBLIC 
	-fsanitize=address
)
target_link_options(${PROJECT_NAME} PUBLIC 
	-fsanitize=address
)
set_source_files_properties(native/dx12_nri.cpp PROPERTIES
    INCLUDE_DIRECTORIES "${DirectX_INCLUDE_DIR}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ../
)

if (MSVC)
    message(STATUS "MSVC detected")
    target_compile_options(${PROJECT_NAME} PRIVATE "/Zc:__cplusplus")
    #need to deploy dll-s using windeployqt
    if(${CMAKE_GENERATOR} STREQUAL Ninja)
        message(STATUS "Visual Studio detected")
        message(STATUS ${QT6_DIR}/../../bin/windeployqt.exe ${CMAKE_BINARY_DIR}/../${PROJECT_NAME}.exe)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
            ${QT6_DIR}/../../bin/windeployqt.exe ${CMAKE_BINARY_DIR}/../${PROJECT_NAME}.exe
        )
    else()
        message(STATUS ${QT6_DIR}/../../bin/windeployqt.exe ${CMAKE_BINARY_DIR}/../Debug/${PROJECT_NAME}.exe)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
            ${QT6_DIR}/../../bin/windeployqt.exe ${CMAKE_BINARY_DIR}/../Debug/${PROJECT_NAME}.exe
        )
    endif()
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    # path to the root directory of the project
    PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}/"
)


