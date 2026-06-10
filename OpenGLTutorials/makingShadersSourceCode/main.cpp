#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <openglErrorReporting.h>
#include <shader.h>




float triangleData[] = {
	//positions      colors
	//x  y  z        r  g  b
	  0, 1, 0,       1, 0, 0, // vertex 1
	  -1, -1, 0,     0, 1, 0, // vertex 2
	  1, -1, 0,      0, 0, 1  // vertex 3
};


int main(void)
{


	if (!glfwInit())
	{
		std::cout << "GLFW Init Error!\n";
		return 1;
	}

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

	GLFWwindow *window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		std::cout << "Window Error!\n";
		return 1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return 1;
	}

	enableReportGlErrors();

#pragma region buffer
	//create the buffer
	GLuint buffer = 0;
	glGenBuffers(1, &buffer);

	//send the data to the buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleData), triangleData, GL_STATIC_DRAW);

	//the atribute representing position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float)*3));

#pragma endregion

#pragma region shader loading
	Shader shader;

	shader.loadShaderProgramFromFile("resources/myshader.vert", "resources/myshader.frag");

	shader.bind();
#pragma endregion


	while (!glfwWindowShouldClose(window))
	{

		glClear(GL_COLOR_BUFFER_BIT);

		
		glDrawArrays(GL_TRIANGLES, 0, 3);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}

