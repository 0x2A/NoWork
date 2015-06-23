#pragma once

#include "Texture.h"


class Texture2D : public Texture
{
public:

	NOWORK_API static Texture2D* Create(unsigned int width, unsigned int height, Texture::Format format, const unsigned char *pixels, bool constant = true);

	NOWORK_API void Update(const unsigned char* pixels);
protected:

	Texture2D();

private:

	int m_InternalFormat;
	int m_Type;
	Texture::Format m_Format;
	bool m_Constant;
};