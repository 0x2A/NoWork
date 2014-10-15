#include "Framework.h"

#include "Input.h"
#include "EventHandler.h"
#include "Game.h"
#include "Log.h"

NoWork::NoWork()
{
	static Log log; //trick to call Logs constructor

	LOG_MESSAGE("NoWork framework version: " << NOWORK_VERSION);

	if (!glfwInit())
	{
		std::cout << "ERROR: Failed to initialize glfw" << std::endl;
	}

	glfwSetErrorCallback(&EventHandler::ErrorCallback);

	m_GameHandle = NULL;
}

NoWork::~NoWork()
{

	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

bool NoWork::CreateNewWindow(std::string title, int width, int height, int posX, int posY, int flags)
{
	m_WindowFlags = flags;

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, flags & Window::Flags::WINDOW_OPENGL_FORWARD_COMPAT);
	glfwWindowHint(GLFW_RESIZABLE, flags & Window::Flags::WINDOW_RESIZABLE);
	glfwWindowHint(GLFW_VISIBLE, flags & Window::Flags::WINDOW_VISIBLE);
	glfwWindowHint(GLFW_DECORATED, flags & Window::Flags::WINDOW_DECORATED);
	glfwWindowHint(GLFW_STEREO, flags & Window::Flags::WINDOW_STEREO_RENDERING);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, flags & Window::Flags::WINDOW_OPENGL_DEBUG_CONTEXT);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_OpenGLMajorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_OpenGLMinorVersion);

	if (m_CoreProfileOnly)
	{
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	m_Window = glfwCreateWindow(width, height, title.c_str(), (flags & Window::Flags::WINDOW_FULLSCREEN) ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (!m_Window)
	{
		return false;
	}
	if (!(flags & Window::Flags::WINDOW_FULLSCREEN)) 
		glfwSetWindowPos(m_Window, posX, posY);

	glfwMakeContextCurrent(m_Window);
	
	m_GlContext = glfwGetCurrentContext();
	

	if (gl3wInit()) {
		LOG_ERROR("failed to initialize gl3w");
		return false;
	}

	//Print window informations of actual settings
	int w, h;
	glfwGetWindowSize(m_Window, &w, &h);
	GLFWmonitor* monitor = glfwGetWindowMonitor(m_Window);
	
	LOG_MESSAGE("Created Window:\n" << "Dimension: " << w << "x" << h << "\n" <<
		"Mode: " << ((monitor == 0) ? "windowed" : "fulscreen") << "\n" <<
		"OpenGL version: " << glGetString(GL_VERSION) << ", GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION));

	Input::Init(m_Window);
#ifdef _DEBUG
	glfwSetKeyCallback(m_Window, EventHandler::KeyEventCallback);
#endif

	if (ExtensionAvailable("GL_ARB_debug_output"))
	{
		glDebugMessageCallbackARB(EventHandler::GLErrorCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT);
	}
	
	return true;
}

bool NoWork::ExtensionAvailable(std::string name)
{
	if (!glfwExtensionSupported(name.c_str()))
	{
		LOG_WARNING(name << " not supported!");
		return false;
	}
	return true;
}

void NoWork::Run()
{

	if (!m_GameHandle)
	{
		std::cout << "ERROR: No game registered! Please call RegisterGame and pass a valid game object pointer." << std::endl;
		Exit();
	}

	//Initialize the game
	m_GameHandle->m_Framework = this;
	m_GameHandle->Init();

	while (!glfwWindowShouldClose(m_Window))
	{
		Input::Update();

		m_GameHandle->OnUpdate(0);
		m_GameHandle->OnRender();

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	m_GameHandle->OnShutdown();
}

void NoWork::Exit()
{
	glfwSetWindowShouldClose(m_Window, true);
}