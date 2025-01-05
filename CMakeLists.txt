cmake_minimum_required(VERSION 3.31.2)

project(Practice)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_executable(Practice src/main.cpp)
# Include all .cpp files in the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

set(GLAD_SOURCES external/glad/src/glad.c)

find_package(OpenGL REQUIRED)

add_executable(Practice ${SOURCES} ${GLAD_SOURCES})

target_include_directories(Practice PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/external/glad/include ${OPENGL_INCLUDE_DIR})
target_link_directories(Practice PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(Practice PRIVATE mingw32 SDL2main SDL2 OpenGL::GL)

