Compile the glfw files if not already compiled:

Add all the libraries and object files with the following command and compile:

` g++ main.cpp openglErrorReporting.cpp ./third-party/glad/src/glad.c -I ./third-party/glad/include/ -I ./third-party/glfw-3.4/include/ -L ./third-party/glfw-3.4/src/ -lglfw3 -lGL -ldl`

