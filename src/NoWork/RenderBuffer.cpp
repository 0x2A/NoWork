#include "NoWork/RenderBuffer.h"
#include "NoWork/NoWork.h"


struct AM_AttachToFBOParam
{
	unsigned int target;
	unsigned int type;
};

glm::ivec2 RenderBuffer::GetSize()
{
	return glm::ivec2(m_Width, m_Height);
}

unsigned int RenderBuffer::GetObjectId()
{
	return m_BufferId;
}

NOWORK_API RenderBufferPtr RenderBuffer::Create(int width, int height, Format bufferFormat, int samples)
{
	RenderBufferPtr b = RenderBufferPtr(new RenderBuffer());
	b->m_Width = width;
	b->m_Height = height;
	b->m_Format = bufferFormat;
	b->m_Samples = samples;

	if (!NoWork::IsMainThread())
		b->AddToGLQueue(RenderBuffer::AM_GenerateBufferObject, nullptr);
	else
		b->GenerateBufferObject();
	return b;
}

void RenderBuffer::GenerateBufferObject()
{
	if (!NoWork::IsMainThread())
	{
		AddToGLQueue(AsyncMode_t::AM_GenerateBufferObject);
		return;
	}

	glCreateRenderbuffers(1, &m_BufferId);

	if(m_Samples > 0)
		glNamedRenderbufferStorageMultisample(m_BufferId, m_Samples, m_Format, m_Width, m_Height);
	else
		glNamedRenderbufferStorage(m_BufferId, m_Format, m_Width, m_Height);
}

void RenderBuffer::AttachToFBO(unsigned int target, unsigned int type)
{
	if (!NoWork::IsMainThread())
	{
		AM_AttachToFBOParam* p = new AM_AttachToFBOParam();
		p->target = target;
		p->type = type;
		AddToGLQueue(AsyncMode_t::AM_AttachToFBO, p);
		return;
	}
	glNamedFramebufferRenderbuffer(target, type, GL_RENDERBUFFER, m_BufferId);
}

void RenderBuffer::DoAsyncWork(int mode, void *params)
{
	switch ((AsyncMode_t)mode)
	{
	case RenderBuffer::AM_GenerateBufferObject:
		GenerateBufferObject();
		break;
	case RenderBuffer::AM_AttachToFBO:
		AM_AttachToFBOParam* p = (AM_AttachToFBOParam*)params;
		AttachToFBO(p->target, p->type);
		delete p;
		break;
	}
}
