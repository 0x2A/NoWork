#pragma once

#include "NoWork/Common.h"
#include "NoWork/Texture.h"


class RenderTexture : public Texture
{
	friend class Framebuffer;
public:

	enum Type : unsigned int
	{
		TEXTURE_1D = 0x0DE0,
		TEXTURE_2D = 0x0DE1,
		TEXTURE_3D = 0x0DE2,
	};

	NOWORK_API static RenderTexture* Create(int width, int height, Type type, Texture::Format textureFormat, bool compressed = false);

protected:

	NOWORK_API RenderTexture(unsigned int texType = GL_TEXTURE_2D, unsigned int texBindingType = GL_TEXTURE_BINDING_2D);
private:

	int m_InternalFormat;
	int m_Type;
	Texture::Format m_Format;
};