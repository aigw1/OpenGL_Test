#include <iostream>
#define GLEW_STATIC
#include <glew.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

#include "defines.h"
#include "vertex_buffer.h"
#include "shader.h"
#include "index_buffer.h"
#include <cmath>
#include "libs/glm/glm/glm.hpp"
#include "libs/glm/glm/ext/matrix_transform.hpp"
#include "libs/glm/glm/gtc/matrix_transform.hpp"
#include "modelLoader.h"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#include "floating_camera.h"
using namespace std;

#ifdef _DEBUG
void GLAPIENTRY openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	cout << "OpenGL error: " << message << endl;
}
#endif

int main(int argc, char** argv) {
	SDL_Window* window; //pointer onto the window
	SDL_Init(SDL_INIT_EVERYTHING); //initalizes everything
	SDL_GL_SetSwapInterval(1); //VSync 1, VSync aus 0, GSync -1

#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	uint32 flags = SDL_WINDOW_OPENGL;

	window = SDL_CreateWindow("C++ OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, flags); //creates window
	SDL_GLContext context = SDL_GL_CreateContext(window); //creates context for the window

	GLenum err = glewInit(); //Initalizes Glew
	if (err != GLEW_OK) { //Error?
		cout << "Error:" << glewGetErrorString(err) << endl;
		cin.get();
		return -1;
	}
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLDebugCallback, 0);
#endif

	Vertex vertices[] = {
		Vertex{-0.75f, -0.75f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f },
		Vertex{ -0.75f, 0.75f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f },
		Vertex{ 0.75f, -0.75f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f },
		Vertex{ 0.75f, 0.75f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f },
	};
	uint32 numVertices = 4;

	uint32 indices[] = {
		0, 1, 2,
		1, 2, 3
	};
	uint32 numIndices = 6;

	std::vector <Vertex> vert;
	std::vector <uint32> ind;

	ObjLoader("model/monkey.obj", &vert, &ind);
	std::cout << "vertecies: " << vert.size() << std::endl;
	std::cout << "indices: " << ind.size() << std::endl;
	for (int i = 0; i < vert.size(); i++) {
		vert[i].r = 1.0f;
		vert[i].g = 0.0f;
		vert[i].b = 0.0f;
		vert[i].a = 1.0f;
	}

	uint32* tempInd;
	Vertex* tempVert;
	tempInd = new uint32[ind.size()];
	tempVert = new Vertex[vert.size()];
	for (int i = 0; i < ind.size(); i++) {
		tempInd[i] = ind.at(i) - 1;
	}
	

	for (int i = 0; i < vert.size(); i++) {
		tempVert[i].x = vert.at(i).x;
		tempVert[i].y = vert.at(i).y;
		tempVert[i].z = vert.at(i).z;
	}

	for (int i = 0; i < vert.size(); i++)
		std::cout << tempVert[i].x << " " << tempVert[i].y << " " << tempVert[i].z << std::endl;

	for (int i = 0; i < ind.size(); i+=3)
		std::cout << tempInd[i] << tempInd[i + 1] << tempInd[i + 2] << std::endl;

	std::cout << tempVert[7].x << tempVert[7].y << tempVert[7].z << std::endl;

	IndexBuffer indexBuffer(tempInd, ind.size(), sizeof(uint32));

	VertexBuffer vertexBuffer(tempVert, vert.size());
	vertexBuffer.unbind();

	/*int32 textureWidth = 0;
	int32 textureHeight = 0;
	int32 bitsPerPixel = 0;
	stbi_set_flip_vertically_on_load(true);
	auto textureBuffer = stbi_load("graphics/animemistake.png", &textureWidth, &textureHeight, &bitsPerPixel, 4);

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_TEXTURE_MIN_FILTER und GL_TEXTURE_MAG_FILTER wird ben�tigt um Texturen in verschieden gr��en anzeigen zu lassen
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //Man n�hrt sich der Texture an MAG texture wird gr��er Min texture wird kleiner
	//Mittels GL_NEAREST kann man einen pixelgen minecraft look erziehlen
	//Statt GL_LINEAR verw�scht texturen (smoothe �berg�nge zwischen pixeln, Farben)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);//unbinds Buffer

	if (textureBuffer) {
		stbi_image_free(textureBuffer);
	}*/

	Shader shader("shaders/basic.vs", "shaders/basic.fs");
	shader.bind();

	uint64 perfCounterFrequency = SDL_GetPerformanceFrequency();
	uint64 lastCounter = SDL_GetPerformanceCounter();
	float32 delta = 0.0f;

	int colorUniformLocation = glGetUniformLocation(shader.getShaderId(), "u_color");
	if (!colorUniformLocation != -1) {
		glUniform4f(colorUniformLocation, 1.0f, 0.0f, 0.0f, 1.0f);
	}

/*	int textureUniformLocation = glGetUniformLocation(shader.getShaderId(), "u_texture");
	if (!textureUniformLocation != -1) {
		glUniform1i(textureUniformLocation, 0);
	}*/

	//Wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

	FloatingCamera camera(90.0f, 800.0f, 600.0f);
	camera.translate(glm::vec3(0.0f, 0.0f, 5.0f));
	camera.update();

	int modelViewProjMatrixLocation = glGetUniformLocation(shader.getShaderId(), "u_modelViewProj");

	bool ButtonW = false;
	bool ButtonS = false;
	bool ButtonA = false;
	bool ButtonD = false;
	bool ButtonShift = false;
	bool ButtonSpace = false;
	bool ButtonEsc = false;

	float cameraSpeed = 6.0f;
	float time = 0.0f;
	bool close = false;
	SDL_SetRelativeMouseMode(SDL_TRUE);
	while (!close) { //Game-loop

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				close = true;

			else if (event.type == SDL_KEYDOWN)
				switch (event.key.keysym.sym) {
				case SDLK_w:
					ButtonW = true;
					break;
				case SDLK_s:
					ButtonS = true;
					break;
				case SDLK_a:
					ButtonA = true;
					break;
				case SDLK_d:
					ButtonD = true;
					break;
				case SDLK_LSHIFT:
					ButtonShift = true;
					break;
				case SDLK_SPACE:
					ButtonSpace = true;
					break;
				case SDLK_1:
					ButtonEsc = true;
					break;
				}

			else if (event.type == SDL_KEYUP)
				switch (event.key.keysym.sym) {
				case SDLK_w:
					ButtonW = false;
					break;
				case SDLK_s:
					ButtonS = false;
					break;
				case SDLK_a:
					ButtonA = false;
					break;
				case SDLK_d:
					ButtonD = false;
					break;
				case SDLK_LSHIFT:
					ButtonShift = false;
					break;
				case SDLK_SPACE:
					ButtonSpace = false;
					break;
				}

			else if (event.type == SDL_MOUSEMOTION)
				camera.onMouseMoved(event.motion.xrel, event.motion.yrel);
		}

		glClearColor(0.2f, 0.1f, 0.0f, 1.0f); //initalizes Color buffer
		glClear(GL_COLOR_BUFFER_BIT); //Clears window with the set color
		time += delta;

		if (ButtonW) 
			camera.moveFront(delta * cameraSpeed);
		if (ButtonS) 
			camera.moveFront(-delta * cameraSpeed);
		if (ButtonA)
			camera.moveSideways(-delta * cameraSpeed);
		if (ButtonD)
			camera.moveSideways(delta * cameraSpeed);
		if (ButtonSpace)
			camera.moveUp(delta * cameraSpeed);
		if (ButtonShift)
			camera.moveUp(-delta * cameraSpeed);
		if (ButtonEsc)
			return 0;

		camera.update();
	
		if (!colorUniformLocation != -1) {
			glUniform4f(colorUniformLocation, 0.0f, 0.0f, 0.0f, 1.0f);
		}

		model = glm::rotate(model, 1 * delta, glm::vec3(0, 1, 0));

		vertexBuffer.bind();
		indexBuffer.bind();

		glm::mat4 modelViewProj = camera.getViewProj() * model;
		glUniformMatrix4fv(modelViewProjMatrixLocation, 1, GL_FALSE, &modelViewProj[0][0]);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, textureId);
		glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, 0); //GL_UNSIGNED_INT is important
		vertexBuffer.unbind();
		indexBuffer.unbind();

		SDL_GL_SwapWindow(window);

		uint64 endCounter = SDL_GetPerformanceCounter();
		uint64 counterElapsed = endCounter - lastCounter;
		delta = ((float32)counterElapsed) / (float32)perfCounterFrequency;
		uint32 FPS = (uint32)((float32)perfCounterFrequency / (float32)counterElapsed);
		lastCounter = endCounter;
	//	cout << "FPS: " << FPS << endl;
	}

//	glDeleteTextures(1, &textureId);

	return 0;
}