#pragma once

#include "Common.h"

class GLFWwindow;
class EventHandler
{
public:

	static void ErrorCallback(int error, const char* description);
	static void KeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void APIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam);

private:


};