#include "NoWork/Framebuffer.h"
#include "NoWork/Log.h"

Framebuffer::Framebuffer()
{
	m_FBO = 0;
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_FBO);
}

Framebuffer* Framebuffer::Create()
{
	Framebuffer* fb = new Framebuffer;

	glGenFramebuffers(1, &fb->m_FBO);
	return fb;
}

bool Framebuffer::BindTexture(RenderTexture* renderTexture, AttachmentType targetAttachmentType)
{
	if (!renderTexture)
		return false;

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	switch (renderTexture->GetType())
	{
	case GL_TEXTURE_1D:
		glFramebufferTexture1D(GL_FRAMEBUFFER, targetAttachmentType, renderTexture->GetType(), renderTexture->GetTextureId(), 0);
		break;
	case GL_TEXTURE_2D:
		glFramebufferTexture2D(GL_FRAMEBUFFER, targetAttachmentType, renderTexture->GetType(), renderTexture->GetTextureId(), 0);
		break;
	case GL_TEXTURE_3D:
		glFramebufferTexture3D(GL_FRAMEBUFFER, targetAttachmentType, renderTexture->GetType(), renderTexture->GetTextureId(), 0, 0);
		break;
	default:
		glFramebufferTexture(GL_FRAMEBUFFER, targetAttachmentType, renderTexture->GetTextureId(), 0);
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

	return true;
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
