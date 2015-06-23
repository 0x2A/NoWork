#include "Texture2D.h"
#include "Log.h"

Texture2D::Texture2D() : Texture(GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D)
{

}

Texture2D* Texture2D::Create(GLuint width, GLuint height, Texture::Format format, const GLubyte *pixels, bool constant /*= true*/)
{
	Texture2D* tex = new Texture2D();
	tex->GenerateTexture();  //generate and bind new texture

	tex->m_Format = format;
	tex->m_Width = width;
	tex->m_Height = height;
	tex->m_Constant = constant;

	GetInternalFormat(GL_TEXTURE_2D, format, &tex->m_InternalFormat, &tex->m_Type); //get internal base format and data type for target texture format

	if (m_UseTexStorage && constant) //static and extension available? Use faster method
	{
		glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, tex->m_InternalFormat, tex->m_Type, pixels);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, tex->m_InternalFormat, tex->m_Type, pixels);
	}

	tex->SetLinearTextureFilter(true); //Set texture filtering to linear
	return tex;
}

void Texture2D::Update(const unsigned char* pixels)
{
	if (m_UseTexStorage && m_Constant)
	{
		LOG_WARNING("Unable to update texture: Texture is set to constant and cant be changed");
		return;
	}

	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, m_Format, m_Width, m_Height, 0, m_InternalFormat, m_Type, pixels);
}
