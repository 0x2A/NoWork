#pragma once

#include "nowork/Common.h"
#include "NoWork/RenderTarget.h"
#include "NoWork/RenderTexture.h"
#include "NoWork/RenderBuffer.h"

class Framebuffer;
typedef std::shared_ptr<Framebuffer> FramebufferPtr;

class Framebuffer : public AsyncGLWorker
{
	friend class Renderer;
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
		DEPTH_STENCIL_ATTACHMENT = 0x821A,
		BACK = 0x0405,
	};

	NOWORK_API ~Framebuffer();


	NOWORK_API static FramebufferPtr Create(int width, int height, int samples = 0);

	NOWORK_API bool AttachRenderTarget(RenderTargetPtr renderTarget, AttachmentType targetAttachmentType);
	NOWORK_API void Bind();
	NOWORK_API void BindWrite();
	NOWORK_API void BindRead();
	NOWORK_API void Unbind();
	NOWORK_API RenderTargetPtr GetAttachedRendertarget(AttachmentType at);


	//Most render-targets need to have read access (in 99% of cases you are writing to a render-target with the intent of reading back the results later, often for shadow mapping or post-processing), 
	//so most of the time you should use a texture.
	//But when you don’t need to read the results (using a depth buffer for an off - screen render when you only need to read back the color result, not the depth result) 
	//you should always use a renderbuffer. They are more efficient for that purpose.
	NOWORK_API RenderTexturePtr CreateAndAttachRenderTexture(AttachmentType targetAttachmentType, RenderTexture::Type type, Texture::Format textureFormat, bool compressed = false);
	
	//Most render-targets need to have read access (in 99% of cases you are writing to a render-target with the intent of reading back the results later, often for shadow mapping or post-processing), 
	//so most of the time you should use a texture.
	//But when you don’t need to read the results (using a depth buffer for an off - screen render when you only need to read back the color result, not the depth result) 
	//you should always use a renderbuffer. They are more efficient for that purpose.
	NOWORK_API RenderBufferPtr CreateAndAttachRenderBuffer(AttachmentType targetAttachmentType, RenderBuffer::Format bufferFormat, int samples = 0);

protected:
	Framebuffer();

	virtual void DoAsyncWork(int mode, void *params) override;

private:
	struct AsyncAttachParameters
	{
		RenderTargetPtr renderTexture;
		AttachmentType targetAttachmentType;
		bool forceTextureBinding;
	};

	unsigned int m_FBO;
	glm::ivec2 m_Size;
	int m_Samples;
	std::unordered_map<AttachmentType, RenderTargetPtr> m_BoundTextures;
	std::vector<AttachmentType> m_BoundAttachmentTypes;

};

