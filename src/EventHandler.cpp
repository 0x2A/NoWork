#include "EventHandler.h"

#include "Input.h"
#include "Log.h"


void EventHandler::ErrorCallback(int error, const char* description)
{
	std::cout << "ERROR (" << error << "): " << description << std::endl;
}

void EventHandler::KeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		Input::HandleKey(key);
}


void APIENTRY EventHandler::GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam)
{
	LOG_ERROR("GLError: " << message, __FUNCTION__);
}