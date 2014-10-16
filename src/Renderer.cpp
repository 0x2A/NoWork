#include "Renderer.h"


Renderer::Renderer(GLFWwindow* window) : m_Window(window)
{

	glfwGetFramebufferSize(window, &m_FramebufferWidth, &m_FramebufferHeight);
	m_AspectRatio = m_FramebufferWidth / (float)m_FramebufferHeight;

	glViewport(0, 0, m_FramebufferWidth, m_FramebufferHeight);

	m_Camera = new Camera(this);

}

Renderer::~Renderer()
{
	DelPtr(m_Camera);
}

void Renderer::Update()
{
	if (m_Camera)
		m_Camera->Update();
}
