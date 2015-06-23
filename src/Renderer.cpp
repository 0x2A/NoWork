#include "Renderer.h"


Renderer::Renderer(GLFWwindow* window) : m_Window(window)
{

	glfwGetFramebufferSize(window, &m_FramebufferWidth, &m_FramebufferHeight);
	m_AspectRatio = m_FramebufferWidth / (float)m_FramebufferHeight;

	glViewport(0, 0, m_FramebufferWidth, m_FramebufferHeight);
	SetBackfaceCulling(true);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

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
void Renderer::Render()
{
	m_Camera->Render();
}

void Renderer::ClearScreen(int bitfield /*= ClearBufferBits::STENCIL_BUFFER | ClearBufferBits::COLOR_BUFFER*/)
{
	glClear(bitfield);
}

void Renderer::SetBackfaceCulling(bool state)
{
	state ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}
