#include "Input.h"

#include "Common.h"


std::unordered_map<long, void*> Input::m_KeyHandlersArgs;
std::unordered_map<long, Input::KeyHandler> Input::m_KeyHandlers;
std::unordered_map<long, bool> Input::m_KeyStates;
GLFWwindow* Input::m_Window;


void Input::Init(GLFWwindow* window)
{
	m_Window = window;

	m_KeyHandlers.clear();
}

bool Input::KeyDown(long key)
{
	
	bool keyDown = (glfwGetKey(m_Window, key) != GLFW_RELEASE);
	if (keyDown)
	{
		if (!m_KeyStates[key]) //Key never pressed before, set it to true
		{
			m_KeyStates[key] = true;
			return true;
		}
		//key already pressed
		return false;
	}

	m_KeyStates[key] = false; //key not pressed, reset to false
	return false;
}


bool Input::KeyHeld(long key)
{
	return (glfwGetKey(m_Window, key) == GLFW_PRESS);
}

void Input::Update()
{

}

void Input::HandleKey(long key)
{
	auto gotKey = m_KeyHandlers.find(key);
	if (gotKey != m_KeyHandlers.end())
	{
		m_KeyHandlers[key](m_KeyHandlersArgs[key]);
	}
}

void Input::BindKey(long key, KeyHandler handler, void* args)
{
	auto gotKey = m_KeyHandlers.find(key);
	if (gotKey != m_KeyHandlers.end())
	{
		std::cout << "Warning: Key " << key << " was already bound to a function";
	}
	m_KeyHandlers[key] = handler;
	m_KeyHandlersArgs[key] = args;
}
