
#include "NoWork/RenderTexture.h"
#include "NoWork/Framework.h"


RenderTexturePtr RenderTexture::Create(int width, int height, Type type, Texture::Format textureFormat, bool compressed /*= false*/)
{
	unsigned int tType = type;
	if (textureFormat == GL_DEPTH_COMPONENT || textureFormat == GL_DEPTH_STENCIL)
	{
		tType = GL_TEXTURE_2D;
	}


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
	tex->m_Format = textureFormat;

	if (!NoWork::IsMainThread())
		tex->AddToGLQueue(0, nullptr);
	else
		tex->Generate();
	return tex;
}

unsigned int RenderTexture::GetObjectId()
{
	return m_TextureId;
}

glm::ivec2 RenderTexture::GetSize()
{
	return Texture::GetSize();
}

RenderTexture::RenderTexture(unsigned int texType, unsigned int texBindingType) : Texture(texType, texBindingType)
{}


void RenderTexture::DoAsyncWork(int mode, void *params)
{
	switch (mode)
	{
	case 0:
		Generate();
		break;
	}
}

void RenderTexture::AttachToFBO(unsigned int target, unsigned int type)
{
	glNamedFramebufferTexture(target, type, m_TextureId, 0);
}

void RenderTexture::Generate()
{

	//if(m_Samples == 0 && !(m_Format == GL_DEPTH_COMPONENT || m_Format == GL_DEPTH_STENCIL))
	{
		glCreateTextures(m_TextureType, 1, &m_TextureId);

		//default to linear sampling
		glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//GL_CLAMP_TO_EDGE                  
		glTextureParameterf(m_TextureId, GL_TEXTURE_WRAP_S, 0x812F);
		glTextureParameterf(m_TextureId, GL_TEXTURE_WRAP_T, 0x812F);

		int texIntFrmt = m_Format;
		GetInternalFormat(m_TextureType, m_Format, &texIntFrmt, &m_Type);
		glTextureStorage2D(m_TextureId, 1, m_Format, m_Width, m_Height);
		m_InternalFormat = texIntFrmt;
	}
	/*else
	{
		glCreateRenderbuffers(1, &m_TextureId);

		if(m_Samples > 0)
			glNamedRenderbufferStorageMultisample(m_TextureId, m_Samples, m_Format, m_Width, m_Height);
		else
			glNamedRenderbufferStorage(m_TextureId, m_Format, m_Width, m_Height);
	}*/
}

