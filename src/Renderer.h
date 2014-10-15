#pragma once

#include "Common.h"
#include "Camera.h"

class GLFWwindow;
class Renderer
{
public:
	Renderer(GLFWwindow* window);
	~Renderer();

	NOWORK_API Camera* GetCamera() const { return m_Camera; }
	void GetFramebufferSize(int& width, int& height) { width = m_FramebufferWidth; height = m_FramebufferHeight; }
	float GetAspectRatio() const { return m_AspectRatio; }

	void Update();
	//NOWORK_API void SetCamera(Camera* val) { m_Camera = val; }

private:
	GLFWwindow *m_Window;
	int m_FramebufferWidth, m_FramebufferHeight;
	
	float m_AspectRatio;
	
	Camera* m_Camera;
};