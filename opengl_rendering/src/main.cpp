#include <SDL2/SDL.h>
#include <emscripten.h>
#include <iostream>
#include <GLES3/gl3.h>

SDL_Window* pWindow{ nullptr };
SDL_GLContext glContext{};

// Vertex Shader
const char* vertexShader = 
R"(#version 300 es
	layout(location = 0) in vec3 aPos; //  Explicitly map to layout index 0   
	void main() {
		gl_Position = vec4(aPos, 1.0);
	}
)";

const char* fragmentShader = 
R"(#version 300 es
	precision mediump float;
	out vec4 fragColor;
	void main() {
		fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
)";


GLuint vao{0}, vbo{0}, shaderProgram{0};
GLuint LoadShaderFromMemory(const char* vertexShader, const char* fragmentShader) {

	const GLuint program = glCreateProgram();
	const GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertShader, 1, &vertexShader, nullptr);
	glCompileShader(vertShader);

	GLint status;

	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);


	if(status != GL_TRUE) {
		GLint maxLength;

		glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);
		std::string errorLog(maxLength, ' ');

		glGetShaderInfoLog(vertShader, maxLength, &maxLength, errorLog.data());

		std::cerr << "GLSL Compile failed: " << std::string{errorLog} << std::endl;
		glDeleteShader(vertShader);

		return 0;
	}

	// Fragment shader
	const GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragmentShader, nullptr);
	glCompileShader(fragShader);

	// Query the compile status
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);

	if(status != GL_TRUE) {
		GLint maxLength;

		glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);
		std::string errorLog(maxLength, ' ');

		glGetShaderInfoLog(fragShader, maxLength, &maxLength, errorLog.data());

		std::cerr << "GLSL Compile failed: " << std::string{errorLog} << std::endl;
		glDeleteShader(fragShader);

		return 0;
	}

	if(vertShader == 0 || fragShader == 0) {
		return 0;
	}

	// Attach the shaders to the program
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if(status != GL_TRUE) {
		GLint maxLength;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::string errorLog(maxLength, ' ');

		glGetProgramInfoLog(program, maxLength, &maxLength, errorLog.data());

		std::cerr << "GLSL Link Status: " << std::string{errorLog} << std::endl;
		return 0;
	}

	if(program) {
		return program;
	}

	std::cerr << "Failed to load shader from memory.\n";
	return 0;
}


bool InitSDL()
{
	std::cout << "Initializing SDL...\n";
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
		return false;
	}
	
	// Create the window 
	pWindow = SDL_CreateWindow(
		"SDL2 and Emscripten Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
		
	if (!pWindow)
	{
		std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
		return false;
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	glContext = SDL_GL_CreateContext(pWindow);
	SDL_GL_SetSwapInterval(1);

	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

	// Init
	shaderProgram = LoadShaderFromMemory(vertexShader, fragmentShader);

	if(shaderProgram == 0) {
		std::cerr << "Failed to load shaders.\n";
		return false;
	}

	float vertices[] = {
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,

		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	std::cout << "SDL Initialized Successfully!" << std::endl;
    return true;
}

void CleanUp()
{
	SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();	
}


// void RegisterLuaFunctions() {
// 	lua.open_libraries(sol::lib::base);

// 	lua["Sol2_MoveBox"] = [&](int x, int y) {
// 		boxX = x;
// 		boxY = y;
// 	};

// 	lua["GetBoxX"] = [&]{ return boxX; };
// 	lua["GetBoxY"] = [&]{ return boxY; };
// }

// bool LoadLuaScript()
// {
//     auto result = lua.safe_script_file("/assets/scripts/main.lua");

//     if (!result.valid()) {
//         sol::error err = result;
//         std::cerr << "Lua script failed: " << err.what() << std::endl;
//         return false;
//     }

//     return true;
// }


void GameLoop()
{
	SDL_Event event;
    while (SDL_PollEvent(&event)) 
	{
        if (event.type == SDL_QUIT) 
		{
			// Exit the loop
            emscripten_cancel_main_loop(); 
			CleanUp();
			exit(0);
        }
    }

	// if (!LoadLuaScript()) {
	// 	exit(0);
    // }

	SDL_GL_MakeCurrent(pWindow, glContext);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int width, height;
	SDL_GetWindowSize(pWindow, &width, &height);
	glViewport(0.f, 0.f, width, height);

	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(pWindow);


}

int main()
{
	// std::cout << "Registering Lua Functions...\n";
	// RegisterLuaFunctions();
	// std::cout << "Registered Lua Functions...\n";

 	std::cout << "Starting game...\n";
    if (!InitSDL()) {
		return -1;
	}

   
	std::cout << "Entering game loop...\n";
    emscripten_set_main_loop(GameLoop, 0, 1);

    CleanUp();
	
    return 0;
}
