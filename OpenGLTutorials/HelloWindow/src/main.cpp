#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuff_size_callback(GLFWwindow *window, int height, int width) {
	// first two values set the location of the lower left corner of the window
	glViewport(0, 0, height, width);
}

void processInput(GLFWwindow* window) {
	// If the key is not pressed, it returns GLFW_RELEASE
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}


int main() {

	// GLFW init and GL config settings
	if (!glfwInit()) {
		std::cerr << "Error starting glfw.\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Window creation and context setting
	GLFWwindow *window =
		glfwCreateWindow(800, 600, "Hello Window", nullptr, nullptr);

	if (!window) {
		std::cerr << "Error creating window.\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Load address of GL with glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD.\n";
		return -1;
	}

	// Callback functions
	glfwSetFramebufferSizeCallback(window, framebuff_size_callback);

	float incs[]{1.0f, 0.0f, 0.0f};
	const int MOD = 3;
	int index = 0;

	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Process input
		processInput(window);

		// Sets the color buffer to the given color 
		glClearColor(incs[0], incs[1], incs[2], 1.0f);

		// Retrieves the color buffer as set before
		glClear(GL_COLOR_BUFFER_BIT);

		// 
		incs[index] = 0.0f;
		index = (index + 1) % MOD;
		incs[index] = 1.0f;

		// Swap buffers and listen for events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Terminate
	glfwTerminate();
	return 0;
}