
#include "NoWork/TextureCube.h"
#include "NoWork/Log.h"
#include <soil.h>
#include "NoWork/Framework.h"
#include "NoWork/utils.h"

TextureCube::TextureCube() : Texture(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BINDING_CUBE_MAP)
{

}

TextureCubePtr TextureCube::Create(unsigned int width, unsigned int height, Texture::Format format, CubeMapPixelData pixels, bool constant /*= true*/)
{
	TextureCubePtr tex = TextureCubePtr(new TextureCube);
	tex->GenerateTexture();  //generate and bind new texture

	tex->m_Format = format;
	tex->m_Width = width;
	tex->m_Height = height;
	tex->m_Constant = constant;
	tex->m_Pixels = pixels;
	tex->m_MipMapLevels = 0;
	
	tex->CopyPixelData();

	return tex;
}

NOWORK_API TextureCubePtr TextureCube::Create(unsigned int width, unsigned int height, Texture::Format format, int levels)
{
	TextureCubePtr tex = TextureCubePtr(new TextureCube);
	tex->GenerateTexture();  //generate and bind new texture

	tex->m_Format = format;
	tex->m_Width = width;
	tex->m_Height = height;
	tex->m_MipMapLevels = levels;
	tex->m_Pixels = CubeMapPixelData();
	tex->CopyPixelData();

	return tex;
}

TextureCubePtr TextureCube::Load(const std::string& pathPosX, const std::string& pathNegX, 
	const std::string& pathPosY, const std::string& pathNegY, 
	const std::string& pathPosZ, const std::string& pathNegZ, bool constant /*= true*/)
{
	int width, height, channels;
	CubeMapPixelData pixdata;

	pixdata.posX = SOIL_load_image(pathPosX.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
	if (!pixdata.posX)
	{
		LOG_ERROR("Unable to load cubemap texture '" << pathPosX << "': " << SOIL_last_result());
		return nullptr;
	}
	pixdata.negX = SOIL_load_image(pathNegX.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
	if (!pixdata.negX)
	{
		LOG_ERROR("Unable to load cubemap texture '" << pathNegX << "': " << SOIL_last_result());
		DelPtr(pixdata.posX);
		return nullptr;
	}
	pixdata.posY = SOIL_load_image(pathPosY.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
	if (!pixdata.posY)
	{
		LOG_ERROR("Unable to load cubemap texture '" << pathPosY << "': " << SOIL_last_result());
		DelPtr(pixdata.posX);
		DelPtr(pixdata.negX);
		return nullptr;
	}
	pixdata.negY = SOIL_load_image(pathNegY.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
	if (!pixdata.negY)
	{
		LOG_ERROR("Unable to load cubemap texture '" << pathNegY << "': " << SOIL_last_result());
		DelPtr(pixdata.posX);
		DelPtr(pixdata.negX);
		DelPtr(pixdata.posY);
		return nullptr;
	}
	pixdata.posZ = SOIL_load_image(pathPosZ.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
	if (!pixdata.posZ)
	{
		LOG_ERROR("Unable to load cubemap texture '" << pathPosZ << "': " << SOIL_last_result());
		DelPtr(pixdata.posX);
		DelPtr(pixdata.negX);
		DelPtr(pixdata.posY);
		DelPtr(pixdata.negY);
		return nullptr;
	}
	pixdata.negZ = SOIL_load_image(pathNegZ.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
	if (!pixdata.negZ)
	{
		LOG_ERROR("Unable to load cubemap texture '" << pathNegZ << "': " << SOIL_last_result());
		DelPtr(pixdata.posX);
		DelPtr(pixdata.negX);
		DelPtr(pixdata.posY);
		DelPtr(pixdata.negY);
		DelPtr(pixdata.posZ);
		return nullptr;
	}
	Texture::Format format;
	switch (channels)
	{
	case 1:
		format = Texture::R8;
		break;
	case 2:
		format = Texture::RG8;
		break;
	case 3:
		format = Texture::RGB8;
		break;
	case 4:
		format = Texture::RGBA8;
		break;
	}
	return Create(width, height, format, pixdata, constant);
}

void TextureCube::Update(const CubeMapPixelData& pixels)
{
	Update(pixels, m_Width, m_Height);
}

void TextureCube::Update(const CubeMapPixelData& pixels, int width, int height)
{
	if (m_UseTexStorage && m_Constant)
	{
		LOG_WARNING("Unable to update texture: Texture is set to constant and cant be changed");
		return;
	}

	/*glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, m_Format, width, height, 0, m_InternalFormat, m_Type, pixels.posX);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, m_Format, width, height, 0, m_InternalFormat, m_Type, pixels.negX);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, m_Format, width, height, 0, m_InternalFormat, m_Type, pixels.posY);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, m_Format, width, height, 0, m_InternalFormat, m_Type, pixels.negY);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, m_Format, width, height, 0, m_InternalFormat, m_Type, pixels.posZ);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, m_Format, width, height, 0, m_InternalFormat, m_Type, pixels.negZ);*/

	// face:
	// 0 = positive x face
	// 1 = negative x face
	// 2 = positive y face
	// 3 = negative y face
	// 4 = positive z face
	// 5 = negative z face

	glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0, 0, width, height, 1, m_InternalFormat, m_Type, pixels.posX);
	glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0, 1, width, height, 1, m_InternalFormat, m_Type, pixels.negX);
	glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0, 2, width, height, 1, m_InternalFormat, m_Type, pixels.posY);
	glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0, 3, width, height, 1, m_InternalFormat, m_Type, pixels.negY);
	glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0, 4, width, height, 1, m_InternalFormat, m_Type, pixels.posZ);
	glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0, 5, width, height, 1, m_InternalFormat, m_Type, pixels.negZ);
	
}

void TextureCube::CopyPixelData()
{
	if (!NoWork::IsMainThread())
	{
		AddToGLQueue(AsyncMode_t::AM_CopyPixData);
		return;
	}

	GetInternalFormat(GL_TEXTURE_CUBE_MAP, m_Format, &m_InternalFormat, &m_Type); //get internal base format and data type for target texture format


	if(m_MipMapLevels <= 0)
		m_MipMapLevels = Utility::numMipmapLevels(m_Width, m_Height);	

	if (m_UseTexStorage && m_Constant) //static and extension available? Use faster method
	{
		
		glTextureStorage2D(m_TextureId, m_MipMapLevels, m_Format, m_Width, m_Height);

		// face:
		// 0 = positive x face
		// 1 = negative x face
		// 2 = positive y face
		// 3 = negative y face
		// 4 = positive z face
		// 5 = negative z face
		if(m_Pixels.posX != nullptr)
			glTextureSubImage3D( m_TextureId, m_MipMapLevels, 0, 0, 
				0,   // the offset to desired cubemap face, which offset goes to which face above
				m_Width, m_Height, 1, m_InternalFormat, m_Type, m_Pixels.posX);
		if (m_Pixels.negX != nullptr)
			glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0,
				1,   // the offset to desired cubemap face, which offset goes to which face above
				m_Width, m_Height, 1, m_InternalFormat, m_Type, m_Pixels.negX);
		if (m_Pixels.posY != nullptr)
			glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0,
				2,   // the offset to desired cubemap face, which offset goes to which face above
				m_Width, m_Height, 1, m_InternalFormat, m_Type, m_Pixels.posY);
		if (m_Pixels.negY != nullptr)
			glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0,
				3,   // the offset to desired cubemap face, which offset goes to which face above
				m_Width, m_Height, 1, m_InternalFormat, m_Type, m_Pixels.negY);
		if (m_Pixels.posZ != nullptr)
			glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0,
				4,   // the offset to desired cubemap face, which offset goes to which face above
				m_Width, m_Height, 1, m_InternalFormat, m_Type, m_Pixels.posZ);
		if (m_Pixels.negZ != nullptr)
			glTextureSubImage3D(m_TextureId, m_MipMapLevels, 0, 0,
				5,   // the offset to desired cubemap face, which offset goes to which face above
				m_Width, m_Height, 1, m_InternalFormat, m_Type, m_Pixels.negZ);
	}
	else
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, m_MipMapLevels);
		if(m_Pixels.posX != nullptr)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, m_Format, m_Width, m_Height, 0, m_InternalFormat, m_Type, m_Pixels.posX);
		if(m_Pixels.negX != nullptr)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, m_Format, m_Width, m_Height, 0, m_InternalFormat, m_Type, m_Pixels.negX);
		if(m_Pixels.posY != nullptr)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, m_Format, m_Width, m_Height, 0, m_InternalFormat, m_Type, m_Pixels.posY);
		if(m_Pixels.negY != nullptr)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, m_Format, m_Width, m_Height, 0, m_InternalFormat, m_Type, m_Pixels.negY);
		if(m_Pixels.posZ != nullptr)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, m_Format, m_Width, m_Height, 0, m_InternalFormat, m_Type, m_Pixels.posZ);
		if(m_Pixels.negZ != nullptr)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, m_Format, m_Width, m_Height, 0, m_InternalFormat, m_Type, m_Pixels.negZ);
	}

	SetParameteri( GL_TEXTURE_MIN_FILTER, m_MipMapLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	SetParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if(m_Pixels.posX != nullptr) //only create mipMaps if we had pixel data
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//SetLinearTextureFilter(true); //Set texture filtering to linear

	SOIL_free_image_data(m_Pixels.posX);
	SOIL_free_image_data(m_Pixels.negX);
	SOIL_free_image_data(m_Pixels.posY);
	SOIL_free_image_data(m_Pixels.negY);
	SOIL_free_image_data(m_Pixels.posZ);
	SOIL_free_image_data(m_Pixels.negZ);
}

void TextureCube::DoAsyncWork(int mode, void *params)
{
	Texture::DoAsyncWork(mode, params);

	switch ((AsyncMode_t)mode)
	{
	case TextureCube::AM_CopyPixData:
		CopyPixelData();
		break;
	}
}
