#pragma once

#include "NoWork/Common.h"
#include "NoWork/Texture.h"
#include "NoWork/RenderTarget.h"

class RenderTexture;
typedef std::shared_ptr<RenderTexture> RenderTexturePtr;

class RenderTexture : public Texture, public RenderTarget
{
	friend class Framebuffer;
public:

	enum Type : unsigned int
	{
		TEXTURE_1D = 0x0DE0,
		TEXTURE_2D = 0x0DE1,
		TEXTURE_3D = 0x0DE2,
		TEXTURE_CUBE = 0x8513
	};

	//Most render-targets need to have read access (in 99% of cases you are writing to a render-target with the intent of reading back the results later, often for shadow mapping or post-processing), 
	//so most of the time you should use a texture.
	//But when you don’t need to read the results (using a depth buffer for an off - screen render when you only need to read back the color result, not the depth result) 
	//you should always use a renderbuffer. They are more efficient for that purpose.
	NOWORK_API static RenderTexturePtr Create(int width, int height, Type type, Texture::Format textureFormat, bool compressed = false);



	virtual unsigned int GetObjectId() override;


	virtual glm::ivec2 GetSize() override;

protected:

	NOWORK_API RenderTexture(unsigned int texType = GL_TEXTURE_2D, unsigned int texBindingType = GL_TEXTURE_BINDING_2D);

	virtual void DoAsyncWork(int mode, void *params) override;


	virtual void AttachToFBO(unsigned int target, unsigned int type) override;

private:
	void Generate();

	int m_InternalFormat;
	int m_Type;
	
	bool m_WriteOnly;
};