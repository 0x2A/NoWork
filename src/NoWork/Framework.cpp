#include "nowork/Framework.h"

#include "nowork/Input.h"
#include "nowork/EventHandler.h"
#include "nowork/Game.h"
#include "nowork/Log.h"
#include "nowork/Shader.h"
#include "nowork/Mesh.h"
#include "nowork/Texture.h"
#include "NoWork/AudioSource.h"
#include "NoWork/AudioSystem.h"



typedef struct
{
	unsigned char major;
	unsigned char minor;
} glversion_t;

const glversion_t glVersions[] = { { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 1, 5 }, { 2, 0 }, { 2, 1 }, { 3, 0 }, 
									{ 3, 1 }, { 3, 2 }, { 3, 3 }, 
									{ 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 }, { 4, 4 }, { 4, 5 } };

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
	lastFrame = glfwGetTime();
}

NoWork::~NoWork()
{
	DelPtr(Shader::DefaultUnlit);
	DelPtr(m_Renderer);
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
		LOG_ERROR("OpenGL version " << (int)m_OpenGLMajorVersion << "." << (int)m_OpenGLMinorVersion << " not supported!");
		m_Window = DetectMaxSupportedGlVersionAndCreateWindow(title, width, height, flags & Window::Flags::WINDOW_FULLSCREEN);
		if (!m_Window)
		{
			LOG_ERROR("Fatal error, no supported OpenGL version found!");
			return false;
		}
		LOG_WARNING("Using latest supported OpenGL version: " << (int)m_OpenGLMajorVersion << "." << (int)m_OpenGLMinorVersion << "!");
	}
	if (!(flags & Window::Flags::WINDOW_FULLSCREEN)) 
		glfwSetWindowPos(m_Window, posX, posY);


	glfwMakeContextCurrent(m_Window);

	//glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	//m_LoaderThreadWindow = glfwCreateWindow(1, 1, "Thread Window", 0, m_Window);


	if (gl3wInit()) {
		LOG_ERROR("failed to initialize gl3w");
		return false;
	}

	AudioSystem::Init();

	//Print window informations of actual settings
	int w, h;
	glfwGetWindowSize(m_Window, &w, &h);
	GLFWmonitor* monitor = glfwGetWindowMonitor(m_Window);
	
	LOG_MESSAGE("Created Window:\n" << "Dimension: " << w << "x" << h << "\n" <<
		"Mode: " << ((monitor == 0) ? "windowed" : "fulscreen") << "\n" <<
		"OpenGL version: " << glGetString(GL_VERSION) << ", GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n" <<
		"Hardware vendor: " << glGetString(GL_VENDOR) << "\n" <<
		"Hardware name: " << glGetString(GL_RENDERER));

	Input::Init(m_Window);
#ifdef _DEBUG
	glfwSetKeyCallback(m_Window, EventHandler::KeyEventCallback);
#endif

	if (ExtensionAvailable("GL_ARB_debug_output"))
	{
		glDebugMessageCallbackARB(EventHandler::GLErrorCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT);
	}


	m_Renderer = new Renderer(m_Window);

	//initialize static vars
	Mesh::Init(m_Renderer);
	Texture::Init(this, m_Renderer);

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
	m_Loading = true;
	m_LoadingThread = std::thread(&NoWork::ContentLoaderFunc, this);
	bool m_GameDataLoaded = false;

	while (!glfwWindowShouldClose(m_Window))
	{
		Input::Update();

		if (m_Loading)
		{
			//LOG_DEBUG("loading...");//TODO: Show loading text or something like this
		}
		else
		{
			if (!m_GameDataLoaded)
			{
				Shader::InitializeDefaultShaders();
				m_GameHandle->OnLoadContent();
				m_GameDataLoaded = true;
			}

			Update();

			m_Renderer->Render();
			m_GameHandle->OnRender();
		}

		glfwSwapBuffers(m_Window);
		glfwPollEvents();

		
	}

	m_GameHandle->OnShutdown();
	m_LoadingThread.join();

	AudioSystem::Shutdown();
}


void NoWork::Update()
{
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	AudioSystem::Update();
	m_GameHandle->OnUpdate(deltaTime);

	m_Renderer->Update();
}

void NoWork::Exit()
{
	glfwSetWindowShouldClose(m_Window, true);
}

void NoWork::ContentLoaderFunc()
{
	//We need to create a new OpenGL context (done in glfw by creating a new window, duh)
	//This is needed because in OpenGL only one thread can have access to an context
	//So OpenGL functions in this thread would fail if we wouldnt create a new context
	
	//glfwMakeContextCurrent(m_LoaderThreadWindow);

	m_GameHandle->m_Framework = this;
	m_GameHandle->m_Renderer = m_Renderer;
	
	//Sadly we cant load content here cause Framebuffer objects cant be shared between gl contexts
	//So Mesh creation fails. It would be possible to realize this but this would get too complicated
	//and i dont have the time do care about such sh*t
	m_GameHandle->Init();
	
	m_Loading = false;
}

GLFWwindow* NoWork::DetectMaxSupportedGlVersionAndCreateWindow(std::string title, int width, int height, bool fullscreen)
{
	int index = 0;
	int elCount = sizeof(glVersions) / sizeof(glversion_t);
	for (int i = 0; i < elCount; i++)
	{
		if (m_OpenGLMajorVersion == glVersions[i].major && m_OpenGLMinorVersion == glVersions[i].minor)
		{
			index = i;
			break;
		}
	}

	//disable error callback while we try to get the latest glVersion working
	//so we dont spam the error log
	glfwSetErrorCallback(NULL);

	while (index >= 0)
	{
		--index;
		m_OpenGLMajorVersion = glVersions[index].major;
		m_OpenGLMinorVersion = glVersions[index].minor;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_OpenGLMajorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_OpenGLMinorVersion);

		GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
		if (window)
		{
			glfwSetErrorCallback(&EventHandler::ErrorCallback);
			return window;
		}
	}

	//reenable error callback
	glfwSetErrorCallback(&EventHandler::ErrorCallback);

	return NULL;
}

NOWORK_API void NoWork::EnableVSync()
{
	glfwSwapInterval(1);
}

NOWORK_API void NoWork::DisableVSync()
{
	glfwSwapInterval(0);
}
