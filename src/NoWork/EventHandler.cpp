#include "nowork/EventHandler.h"

#include "nowork/Input.h"
#include "nowork/Log.h"
#include "NoWork/Framework.h"
#include "NoWork/Game.h"


NoWork * EventHandler::m_Framework = NULL;

void EventHandler::ErrorCallback(int error, const char* description)
{
	LOG_ERROR("(" << error << "): " << description);
}

void EventHandler::KeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		Input::HandleKey(key);
}


void APIENTRY EventHandler::GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid *userParam)
{
	std::string strType;
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		strType = "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		strType = "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		strType = "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		strType = "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		strType = "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		strType = "OTHER";
		break;
	}
	std::string strSeverity;
	switch (severity){
	case GL_DEBUG_SEVERITY_LOW:
		strSeverity = "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		strSeverity = "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		strSeverity = "HIGH";
		break;
	}

	LOG_ERROR(strSeverity << " GLError: Type " << strType << ": " << message);
}

void EventHandler::WindowSizeChangedCallback(GLFWwindow* window, int width, int height)
{
	if (m_Framework)
	{
		m_Framework->GetGameHandle()->OnWindowResize(width, height);
	}
}
