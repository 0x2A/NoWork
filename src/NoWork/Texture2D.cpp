#include "nowork/Texture2D.h"
#include "nowork/Log.h"
#include <soil.h>
#include "NoWork/NoWork.h"
#ifdef _NOWORK_INTERNAL_
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#endif
#include "noWork/stb_image.h"


Texture2D::Texture2D() : Texture(GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D)
{

}

  Texture2DPtr Texture2D::Create(unsigned int width, unsigned int height, Texture::Format format, unsigned char *pixels, bool constant /*= true*/)
{
	Texture2DPtr tex = Texture2DPtr(new Texture2D);
	tex->GenerateTexture();  //generate and bind new texture

	tex->m_Format = format;
	tex->m_Width = width;
	tex->m_Height = height;
	tex->m_Constant = constant;
	tex->m_Pixels = pixels;
	tex->m_srgb = (format == Texture::SRGB8 || format == Texture::SRGBA8);
	tex->CopyPixelData();

	return tex;
}

  NOWORK_API Texture2DPtr Texture2D::Create(unsigned int width, unsigned int height, Texture::Format format, int levels /*= 0*/)
  {
	  Texture2DPtr tex = Texture2DPtr(new Texture2D);
	  tex->GenerateTexture();  //generate and bind new texture

	  tex->m_Format = format;
	  tex->m_Width = width;
	  tex->m_Height = height;
	  tex->m_MipMapLevels = levels;
	  tex->m_Pixels = nullptr;
	  tex->m_srgb = (format == Texture::SRGB8 || format == Texture::SRGBA8);
	  tex->CopyPixelData();

	  return tex;
  }

  Texture2DPtr Texture2D::Load(const char* path, bool load_srgb, bool constant /*= true*/)
{
	int width, height, channels;
	
	unsigned char *ht_map = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_AUTO);

	if (!ht_map)
	{
		LOG_ERROR("Unable to load texture '" << path << "': " << SOIL_last_result());
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
		format = load_srgb ? Texture::SRGB8 : Texture::RGB8;
		break;
	case 4:
		format = load_srgb ? Texture::SRGBA8 : Texture::RGBA8;
		break;
	}

	return Create(width, height, format, ht_map, constant);
}

  NOWORK_API Texture2DPtr Texture2D::LoadRawFromMemory(unsigned char* data, unsigned int width, unsigned int height, bool load_srgb /*= true*/, bool constant /*= true*/)
  {
	  if(!data)
	  {
		  LOG_ERROR("Unable to load raw texture from memory: data is NULL");
		  return nullptr;
	  }

	  Texture::Format format = load_srgb ? Texture::SRGBA8 : Texture::RGBA8;
	  return Create(width, height, format, data, constant);
  }

  NOWORK_API Texture2DPtr Texture2D::LoadFromMemory(unsigned char* data, int bufferLength, bool load_srgb /*= true*/, bool constant /*= true*/)
  {
	  int width, height, channels;

	  unsigned char *ht_map = SOIL_load_image_from_memory(data, bufferLength, &width, &height, &channels, SOIL_LOAD_AUTO);

	  if (!ht_map)
	  {
		  LOG_ERROR("Unable to load texture from memory: " << SOIL_last_result());
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
		  format = load_srgb ? Texture::SRGB8 : Texture::RGB8;
		  break;
	  case 4:
		  format = load_srgb ? Texture::SRGBA8 : Texture::RGBA8;
		  break;
	  }

	  return Create(width, height, format, ht_map, constant);
  }

  Texture2DPtr Texture2D::LoadHDR(const char* path)
  {
	  Texture2DPtr tex = Texture2DPtr(new Texture2D);
	  if (!NoWork::IsMainThread())
	  {
		  tex->AddToGLQueue(AsyncMode_t::AM_LoadHDR, (void*)path);
		  return tex;
	  }
	  tex->m_Constant = true;
	  tex->LoadHDRInternal(path);
	  return tex;
  }

void Texture2D::Update(const unsigned char* pixels)
{
	Update(pixels, m_Width, m_Height);
}

void Texture2D::Update(const unsigned char* pixels, int width, int height)
{
	if (m_UseTexStorage && m_Constant)
	{
		LOG_WARNING("Unable to update texture: Texture is set to constant and cant be changed");
		return;
	}

	glTextureSubImage2D(m_TextureId, 0, 0, 0, m_Width, m_Height, m_InternalFormat, m_Type, m_Pixels);
}


void Texture2D::DoAsyncWork(int mode, void *params)
{
	Texture::DoAsyncWork(mode, params);

	switch ((AsyncMode_t)mode)
	{
	case Texture2D::AM_CopyPixData:
		CopyPixelData();
		break;
	case Texture2D::AM_LoadHDR:
		LoadHDRInternal((const char*)params);
		break;
	}
}

void Texture2D::LoadHDRInternal(const char* path)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);

	if (!data)
	{
		LOG_ERROR("Unable to load hdr texture '" << path << "'");
		return;
	}

	m_Width = width;
	m_Height = height;

	m_MipMapLevels = Utility::numMipmapLevels(width, height);

	//somehow Direct access functions dont work with GL_FLOAT textures
	glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);

	glTextureStorage2D(m_TextureId, m_MipMapLevels, GL_RGB16F, width, height);

	SetParameterf(GL_TEXTURE_MIN_FILTER, m_MipMapLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	SetParameterf(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureSubImage2D(m_TextureId, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);

	SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	if (m_MipMapLevels > 1) {
		glGenerateTextureMipmap(m_TextureId);
	}

	stbi_image_free(data);
}

void Texture2D::CopyPixelData()
{
	if (!NoWork::IsMainThread())
	{
		AddToGLQueue(AsyncMode_t::AM_CopyPixData);
		return;
	}

	if(m_MipMapLevels <= 0)
		m_MipMapLevels = Utility::numMipmapLevels(m_Width, m_Height);

	GetInternalFormat(GL_TEXTURE_2D, m_Format, &m_InternalFormat, &m_Type); //get internal base format and data type for target texture format

	//glBindTexture(GL_TEXTURE_2D, m_TextureId);

	glTextureStorage2D(m_TextureId, m_MipMapLevels, m_Format, m_Width, m_Height);
	if (m_Pixels != nullptr)
		glTextureSubImage2D(m_TextureId, 0, 0, 0, m_Width, m_Height, m_InternalFormat, m_Type, m_Pixels);


	SetParameteri( GL_TEXTURE_BASE_LEVEL, 0);
	SetParameteri( GL_TEXTURE_MAX_LEVEL, m_MipMapLevels);

	SetLinearTextureFilter(true); //Set texture filtering to linear

	SOIL_free_image_data(m_Pixels);
}
