#include "context.h"
// system libarays
#include <cstdio>
#include <iostream>
#include <cassert>
bool context::init(int width, int height, const char* title)
{
	// initialize glfw
	int glfwStatus = glfwInit();
	if(glfwStatus == GLFW_FALSE)
	{
		printf("An error occurred with GLFW.");
		return false;
	}
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	glfwMakeContextCurrent(window);
	// initialize glew
	int glewStatus = glewInit();
	if (glewStatus != GLEW_OK)
	{
		printf("An error occurred with GLEW.");
		return false;
	}

	// print out some diagnostics

	// GL_VERSION
	// GL_RENDERER
	// GL_VENDOR
	// GL_SHADING_LANGUAGE_VERSION

	printf("OpenGL Version: %s\n", (const char*) glGetString(GL_VERSION));
	printf("Renderer: %s\n", (const char*)glGetString(GL_RENDERER));
	printf("Vendor: %s\n", (const char*)glGetString(GL_VENDOR));
	printf("GLSL: %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	// add in error logging logic
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true);
	// set up some good defaults
	glEnable(GL_CULL_FACE);   // ogl defaults to off
	glCullFace(GL_BACK);      // cull back faces
	glFrontFace(GL_CCW);      // ccw faces are front faces (ogl defaults to CCW)

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	// setup some good defaults
	glClearColor(0.25f,0.25f,0.25f,1.0f);

	// return true if everything is OK
	return true;
}

void context::tick()
{
	// update glfw
	glfwPollEvents(); // update events,input, etc.
	glfwSwapBuffers(window); // update window (the one in Windows)
}

float context::time() const
{
	return (float)glfwGetTime();
}

void context::term()
{
	// clean up our window
	glfwDestroyWindow(window);
	window = nullptr;

	// clean up GLFW
	glfwTerminate();
}

void context::clear()
{
	// clears the screen and replaces it w/ the color
	// giveb by the glClearColor function call
	glClear(GL_COLOR_BUFFER_BIT);
}

bool context::shouldClose() const
{
	return glfwWindowShouldClose(window);
}

void context::resetTime(float resetValue)
{
	glfwSetTime(resetValue);
}

void context::errorCallback
(GLenum source, 
	GLenum type, 
	GLenum id, 
	GLenum severity, 
	GLsizei length, 
	const GLchar* message, 
	const void* userParam
)
{
	printf(message, "\n");
}
