#pragma once

#include "Common.h"
#include "Camera.h"




struct GLFWwindow;
class Texture;
class Renderer
{
public:

	enum ClearBufferBits
	{
		DEPTH_BUFFER = 0x00000,
		STENCIL_BUFFER = 0x00000400,
		COLOR_BUFFER = 0x00004000
	};

	enum AttachmentType
	{
		COLOR0 = 0x8CE0,
		COLOR1,
		COLOR2,
		COLOR3,
		COLOR4,
		COLOR5,
		COLOR6,
		COLOR7,
		COLOR8,
		COLOR9,
		COLOR10,
		COLOR11,
		COLOR12,
		COLOR13,
		COLOR14,
		COLOR15,
		DEPTH_ATTACHMENT = 0x8D00,
		STENCIL_ATTACHMENT = 0x8D20,
		DEPTH_STENCIL_ATTACHMENT = 0x821A
	};

	Renderer(GLFWwindow* window);
	~Renderer();

	NOWORK_API Camera* GetCamera() const { return m_Camera; }
	NOWORK_API void GetFramebufferSize(int& width, int& height) { width = m_FramebufferWidth; height = m_FramebufferHeight; }
	NOWORK_API float GetAspectRatio() const { return m_AspectRatio; }
	NOWORK_API GLFWwindow * GetWindow() const { return m_Window; }

	void Update();
	void Render();

	NOWORK_API void SetBackfaceCulling(bool state);

	NOWORK_API void SetAlphaBlending(bool state);
	NOWORK_API void SetWireframeMode(bool state);

	NOWORK_API Texture* CreateFBOTexture(int width, int height, unsigned int textureFormat, bool compressed = false);
	NOWORK_API unsigned int CreateFrameBuffer();
	NOWORK_API bool BindTextureToFrameBuffer(unsigned int framebuffer, Texture *texture, AttachmentType targetAttachmentType);
	NOWORK_API void BindFrameBuffer(unsigned int framebuffer);
	NOWORK_API void UnBindFrameBuffer();

	NOWORK_API void SetViewPort(int x, int y, int width, int height);

	NOWORK_API void ClearScreen(int bitfield = ClearBufferBits::STENCIL_BUFFER | ClearBufferBits::COLOR_BUFFER);

	//NOWORK_API void SetCamera(Camera* val) { m_Camera = val; }

private:

	GLFWwindow *m_Window;
	
	int m_FramebufferWidth, m_FramebufferHeight;
	
	float m_AspectRatio;
	
	Camera* m_Camera = NULL;
};