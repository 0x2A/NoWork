
#include "NoWork/RenderTexture.h"


RenderTexturePtr RenderTexture::Create(int width, int height, Type type, Texture::Format textureFormat, bool compressed /*= false*/)
{
	unsigned int tType = type;
	if (textureFormat == GL_DEPTH_COMPONENT || textureFormat == GL_DEPTH_STENCIL)
	{
		tType = GL_TEXTURE_2D;
	}
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(tType, texture);

	glTexParameteri(tType, GL_TEXTURE_MIN_FILTER, 0x2600);
	glTexParameteri(tType, GL_TEXTURE_MAG_FILTER, 0x2600);

	glTexParameterf(tType, GL_TEXTURE_WRAP_S, 0x812F);
	glTexParameterf(tType, GL_TEXTURE_WRAP_T, 0x812F);

	if (textureFormat == GL_DEPTH_COMPONENT || textureFormat == GL_DEPTH_STENCIL)
	{
		// This is to allow usage of shadow2DProj function in the shader
		glTexParameteri(tType, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(tType, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}

	int texIntFrmt = textureFormat;
	int texFrmt = GL_RGB;

	GetInternalFormat(tType, textureFormat, &texIntFrmt, &texFrmt);

	glTexImage2D(tType, 0, textureFormat, width, height, 0, texIntFrmt, texFrmt, 0);
	glBindTexture(tType, 0);

	unsigned int bindingType;
	//get binding type
	switch (tType)
	{
	case GL_TEXTURE_1D:
		bindingType = GL_TEXTURE_BINDING_1D;
		break;
	case GL_TEXTURE_3D:
		bindingType = GL_TEXTURE_BINDING_3D;
		break;
	case GL_TEXTURE_2D:
	default:
		bindingType = GL_TEXTURE_BINDING_2D;
		break;
	}

	RenderTexturePtr tex = RenderTexturePtr(new RenderTexture(tType, bindingType));
	tex->m_Width = width;
	tex->m_Height = height;
	tex->m_TextureId = texture;
	tex->m_Format = (Texture::Format)texIntFrmt;
	tex->m_Type = texFrmt;
	tex->m_InternalFormat = texIntFrmt;

	return tex;
}

RenderTexture::RenderTexture(unsigned int texType, unsigned int texBindingType) : Texture(texType, texBindingType)
{}


