cmake_minimum_required(VERSION 3.5)
project(OpenGL_opening_window VERSION 1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files
set(SOURCES
    #openglErrorReporting.cpp
    ./src/main.cpp
    ./third-party/glad/src/glad.c
)

# Define the executable target
add_executable(OpenGL_Window ${SOURCES})


# Specify header include directories (-I flag)
target_include_directories(OpenGL_Window PRIVATE
    ./third-party/glad/include/
    ./third-party/glfw-3.4/include/
)

# Specify library search directories (-L flags)
target_link_directories(OpenGL_Window PRIVATE
    ./third-party/glfw-3.4/src/
)


# Link the actual libraries
# On Linux, CMake natively recognizes GL and dl
target_link_libraries(OpenGL_Window PRIVATE
    glfw3
    GL
    dl
)