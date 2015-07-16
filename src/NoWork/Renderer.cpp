#include "nowork/Renderer.h"
#include "nowork/Texture.h"
#include "nowork/Texture2D.h"
#include "nowork/Log.h"
#include "NoWork/Framework.h"

//gl3w is missing this, idk why...
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

Renderer::Renderer(NoWork* framework, GLFWwindow* window) : m_Window(window)
{
	m_Framework = framework;
	glfwGetFramebufferSize(window, &m_FramebufferWidth, &m_FramebufferHeight);
	m_AspectRatio = m_FramebufferWidth / (float)m_FramebufferHeight;

	glViewport(0, 0, m_FramebufferWidth, m_FramebufferHeight);
	SetBackfaceCulling(true);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	m_Camera = new Camera(this);

	m_AnisotropicFilteringAvail = m_Framework->ExtensionAvailable("GL_EXT_texture_filter_anisotropic");
	m_AnisotropicFilteringVal = 0.0f;

	if (m_AnisotropicFilteringAvail)
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_MaxAnisotropicFiltering);
	}
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

void Renderer::SetAlphaBlending(bool state)
{
	if (state)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);
}

void Renderer::SetWireframeMode(bool state)
{
	if (state)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

unsigned int Renderer::CreateFrameBuffer()
{
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	return fbo;
}

bool Renderer::BindTextureToFrameBuffer(unsigned int framebuffer, Texture *texture, AttachmentType targetAttachmentType)
{
	if (!texture)
		return false;

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	switch (texture->GetType())
	{
	case GL_TEXTURE_1D:
		glFramebufferTexture1D(GL_FRAMEBUFFER, targetAttachmentType, texture->GetType(), texture->GetTextureId(), 0);
		break;
	case GL_TEXTURE_2D:
		glFramebufferTexture2D(GL_FRAMEBUFFER, targetAttachmentType, texture->GetType(), texture->GetTextureId(), 0);
		break;
	case GL_TEXTURE_3D:
		glFramebufferTexture3D(GL_FRAMEBUFFER, targetAttachmentType, texture->GetType(), texture->GetTextureId(), 0, 0);
		break;
	default:
		glFramebufferTexture(GL_FRAMEBUFFER, targetAttachmentType, texture->GetTextureId(), 0);
	}

	// check FBO status
	GLenum FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (FBOstatus != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("GLError: GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");
		return false;
	}

	// switch back to window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::BindFrameBuffer(unsigned int framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Renderer::UnBindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetViewPort(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

Texture* Renderer::CreateFBOTexture(int width, int height, unsigned int textureFormat, bool compressed /*= false*/)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF. Using GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 0x2600);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 0x2600);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);


	if (textureFormat == GL_DEPTH_COMPONENT || textureFormat == GL_DEPTH_STENCIL)
	{
		// This is to allow usage of shadow2DProj function in the shader
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}

	GLuint texIntFrmt = textureFormat;
	GLuint texFrmt = GL_RGB;

	switch (textureFormat)
	{
	case Texture::DEPTH:
		texIntFrmt = GL_DEPTH_COMPONENT24;
		texFrmt = GL_DEPTH_COMPONENT;
		break;
	case Texture::DEPTH_STENCIL:
		texIntFrmt = GL_DEPTH_STENCIL;
		break;
	case Texture::R8:
	case Texture::R16:
	case Texture::R16F:
	case Texture::R32F:
		texFrmt = GL_RED;
		if (compressed)
			texIntFrmt = GL_COMPRESSED_RED;
		break;
	case Texture::RG8:
	case Texture::RG16:
	case Texture::RG16F:
	case Texture::RG32F:
		texFrmt = GL_RG;
		if (compressed)
			texIntFrmt = GL_COMPRESSED_RG;
		break;
	case Texture::RGB8:
	case Texture::RGB10:
	case Texture::RGB16F:
	case Texture::RGB32F:
		texFrmt = GL_RGB;
		if (compressed)
			texIntFrmt = GL_COMPRESSED_RGB;
		break;
	case Texture::RGBA8:
	case Texture::RGBA16:
	case Texture::RGBA16F:
	case Texture::RGBA32F:
		texFrmt = GL_RGBA;
		if (compressed)
			texIntFrmt = GL_COMPRESSED_RGBA;
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, texIntFrmt, width, height, 0, texFrmt, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	Texture2D *tex = new Texture2D();
	tex->m_Width = width;
	tex->m_Height = height;
	tex->m_TextureId = texture;
	tex->m_Format = (Texture::Format)texIntFrmt;
	tex->m_InternalFormat = texFrmt;
	tex->m_Renderer = this;
	tex->m_Type = GL_TEXTURE_2D;

	return tex;
}

void Renderer::SetAnisotropicFiltering(float val)
{
	if (val <= 0.0f || !m_AnisotropicFilteringAvail) 
		m_AnisotropicFilteringVal = 0.0f;
	else
	{
		m_AnisotropicFilteringVal = val;
		if (m_AnisotropicFilteringVal > m_MaxAnisotropicFiltering)
			m_AnisotropicFilteringVal = m_MaxAnisotropicFiltering;
	}
}

float Renderer::GetAnisotropicFilterValue()
{
	if (!m_AnisotropicFilteringAvail)
		return 0.0f;
	return m_AnisotropicFilteringVal;
}
