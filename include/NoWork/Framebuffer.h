#pragma once

#include "nowork/Common.h"

#include "NoWork/RenderTexture.h"

class Framebuffer;
typedef std::shared_ptr<Framebuffer> FramebufferPtr;

class Framebuffer
{
public:
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

	NOWORK_API ~Framebuffer();

	NOWORK_API static FramebufferPtr Create();

	NOWORK_API bool BindTexture(RenderTexturePtr renderTexture, AttachmentType targetAttachmentType);
	NOWORK_API void Bind();
	NOWORK_API void Unbind();

protected:
	Framebuffer();

private:
	unsigned int m_FBO;
};

