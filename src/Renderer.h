#pragma once

#include "Common.h"
#include "Camera.h"




struct GLFWwindow;
class Renderer
{
public:

	enum ClearBufferBits
	{
		DEPTH_BUFFER = 0x00000,
		STENCIL_BUFFER = 0x00000400,
		COLOR_BUFFER = 0x00004000
	};

	Renderer(GLFWwindow* window);
	~Renderer();

	NOWORK_API Camera* GetCamera() const { return m_Camera; }
	NOWORK_API void GetFramebufferSize(int& width, int& height) { width = m_FramebufferWidth; height = m_FramebufferHeight; }
	NOWORK_API float GetAspectRatio() const { return m_AspectRatio; }
	NOWORK_API GLFWwindow * GetWindow() const { return m_Window; }

	void Update();
	void Render();

	NOWORK_API void ClearScreen(int bitfield = ClearBufferBits::STENCIL_BUFFER | ClearBufferBits::COLOR_BUFFER);

	//NOWORK_API void SetCamera(Camera* val) { m_Camera = val; }

private:
	GLFWwindow *m_Window;
	
	int m_FramebufferWidth, m_FramebufferHeight;
	
	float m_AspectRatio;
	
	Camera* m_Camera = NULL;
};