#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "openglErrorReporting.h"
#include "shader.h"

#include <iostream>

float triData[] = {
	// positions, 	colors
	 1.0,  0.0, 0, 		2.0, 0.0, 0.0,
	-1.0, -1.0, 0, 		0.0, 1.0, 0.0,
	 1.0, -1.0, 0, 		0.0, 0.0, 1.0

};


int main() {
	if(!glfwInit()) {
		std::cerr << "Err initializing GL\n";
		return 1;
	}

	// Create Window
	GLFWwindow *window = glfwCreateWindow(640, 480, "Simple Example", NULL, NULL);

	if(!window) {
		std::cerr << "Err creating window\n";
		return 1;
	}

	// Create Context for the window
	glfwMakeContextCurrent(window);

	// Load opengl
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		return 1;
	}

	enableReportGlErrors();

#pragma region buffer
	// Buffers
	GLuint buffer = 0;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triData), triData, GL_STATIC_DRAW);

	// Configure Attributes : vertices
	glEnableVertexAttribArray(0);
	// 0: first attri, 3: cardinality max 4; type;
	// GL_FALSE: don't convert to float
	// strides for the next data item
	// starting position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);

	// For the second attribute: color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (sizeof(float) * 3));


#pragma endregion

#pragma region shader loading
	Shader shader;

	shader.loadShaderProgramFromFile("resources/shader.vert", "resources/shader.frag");

	shader.bind();
#pragma endregion

	// Game loop
	while(!glfwWindowShouldClose(window)) {
		// Clear the screen; any change takes affect only after calling this
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw 3 vertices and start from the first one (0).
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Resize figure on resizing the window
		int x, y;
		glfwGetWindowSize(window, &x, &y);
		glViewport(0, 0, x, y);

		glfwSwapBuffers(window);
		// This is necessary otherwise the application would become unresponsive
		glfwPollEvents();

	}

	// Destroy Window
	glfwDestroyWindow(window);

	// Terminate glfw
	glfwTerminate();

	return 0;
}