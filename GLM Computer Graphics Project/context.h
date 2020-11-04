#pragma once
// project libaray
#include "glew/GL/glew.h" // must always come before glfw
#include "glfw/GLFW/glfw3.h"
class context
{
	// forward declaration - I am declaring a type and using it here
	struct GLFWwindow* window;
public:
	bool init(int width, int height, const char* title);
	void tick();
	float time() const;
	void term();
	void clear();
	bool shouldClose() const;
	void resetTime(float resetValue);
	void APIENTRY errorCallback
	(
		GLenum source,
		GLenum type,
		GLenum id,
		GLenum severity,
		GLsizei length,
		const GLchar *message,
		const void *userParam
	);

};

