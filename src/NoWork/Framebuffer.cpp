#include "NoWork/Framebuffer.h"
#include "NoWork/Log.h"
#include "NoWork/Framework.h"
#include "NoWork/RenderBuffer.h"

#include <type_traits>

Framebuffer::Framebuffer()
{
	m_FBO = 0;
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_FBO);
}

FramebufferPtr Framebuffer::Create(int width, int height, int samples)
{
	FramebufferPtr fb = FramebufferPtr(new Framebuffer);
	fb->m_Size.x = width;
	fb->m_Size.y = height;
	fb->m_Samples = samples;

	if (!NoWork::IsMainThread())
	{
		fb->AddToGLQueue(0, nullptr);
		return fb;
	}
	glCreateFramebuffers(1, &fb->m_FBO);
	return fb;
}

bool Framebuffer::AttachRenderTarget(RenderTargetPtr renderTarget, AttachmentType targetAttachmentType)
{
	if (!renderTarget)
		return false;

	if (!NoWork::IsMainThread())
	{
		AsyncAttachParameters *params = new AsyncAttachParameters;
		params->renderTexture = renderTarget;
		params->targetAttachmentType = targetAttachmentType;
		AddToGLQueue(1, params);
		return true;
	}

	if (renderTarget->GetSize() != m_Size)
		LOG_WARNING("Bound render target does not match framebuffer size! ObjectID:" << renderTarget->GetObjectId() << " FramebufferId:" << m_FBO);

	
	renderTarget->AttachToFBO(m_FBO, targetAttachmentType);

	//multisample and depth/stencil textures need RBO (render buffer object)

////////////////////////
	/*if(dynamic_cast<RenderTexture*>(renderTarget.get()) == nullptr)
	{
		glNamedFramebufferRenderbuffer(m_FBO, targetAttachmentType, GL_RENDERBUFFER, renderTarget->GetObjectId());
	}
	else
	{
		glNamedFramebufferTexture(m_FBO, targetAttachmentType, renderTarget->GetObjectId(), 0);
	}*/


	// check FBO status
	GLenum FBOstatus = glCheckNamedFramebufferStatus(m_FBO, GL_FRAMEBUFFER);
	if (FBOstatus != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("GLError: GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");
		return false;
	}

	m_BoundTextures[targetAttachmentType] = renderTarget;
	m_BoundAttachmentTypes.clear();
	for (auto &rt : m_BoundTextures)
	{
		if (rt.second && !(rt.first == DEPTH_ATTACHMENT || rt.first == STENCIL_ATTACHMENT || rt.first == DEPTH_STENCIL_ATTACHMENT))
			m_BoundAttachmentTypes.push_back(rt.first);
	}

	// switch back to window-system-provided framebuffer
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	if (m_BoundAttachmentTypes.size() > 0)
		glNamedFramebufferDrawBuffers(m_FBO, m_BoundAttachmentTypes.size(), (GLenum*)&m_BoundAttachmentTypes[0]);
	glViewport(0, 0, m_Size.x, m_Size.y);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTexturePtr Framebuffer::CreateAndAttachRenderTexture(AttachmentType targetAttachmentType,
	RenderTexture::Type type, Texture::Format textureFormat, bool compressed /*= false*/)
{
	RenderTexturePtr tex = RenderTexture::Create(m_Size.x, m_Size.y, type, textureFormat, compressed);
	if (!AttachRenderTarget(std::static_pointer_cast<RenderTarget>(tex), targetAttachmentType))
	{
		LOG_ERROR("Failed to attach texture to Framebuffer: " << targetAttachmentType);
	}
	return tex;
}

NOWORK_API RenderBufferPtr Framebuffer::CreateAndAttachRenderBuffer(AttachmentType targetAttachmentType, RenderBuffer::Format bufferFormat, int samples /*= 0*/)
{
	RenderBufferPtr buf = RenderBuffer::Create(m_Size.x, m_Size.y, bufferFormat, samples);
	if (!AttachRenderTarget(std::static_pointer_cast<RenderTarget>(buf), targetAttachmentType))
	{
		LOG_ERROR("Failed to attach buffer to Framebuffer: " << targetAttachmentType);
	}
	return buf;
}

RenderTargetPtr Framebuffer::GetAttachedRendertarget(AttachmentType at)
{
	return m_BoundTextures[at];
}

void Framebuffer::DoAsyncWork(int mode, void *params)
{
	switch (mode)
	{
	case 0:
		glCreateFramebuffers(1, &m_FBO);
		break;
	case 1:
		AsyncAttachParameters *p = static_cast<AsyncAttachParameters*>(params);
		AttachRenderTarget(p->renderTexture, p->targetAttachmentType);
		DelPtr(p);
		break;
	}
}

NOWORK_API void Framebuffer::BindWrite()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
	if (m_BoundAttachmentTypes.size() > 0)
		glNamedFramebufferDrawBuffers(m_FBO, m_BoundAttachmentTypes.size(), (GLenum*)&m_BoundAttachmentTypes[0]);
	glViewport(0, 0, m_Size.x, m_Size.y);
}

NOWORK_API void Framebuffer::BindRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
}