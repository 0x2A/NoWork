#pragma once

#include "nowork/Texture.h"

class Texture2D;
typedef std::shared_ptr<Texture2D> Texture2DPtr;

class Texture2D : public Texture
{
	friend class Renderer;
public:

	enum AsyncMode_t : int
	{
		AM_CopyPixData = Texture::AM_OFFSET,
		AM_LoadHDR,
		AM_GenMipMaps,
	};

	NOWORK_API static Texture2DPtr Create(unsigned int width, unsigned int height, Texture::Format format, unsigned char *pixels, bool constant = true);
	NOWORK_API static Texture2DPtr Load(const char* path, bool load_srgb = true, bool constant = true); //if load_srgb is true, images will be loaded gamma corrected. For lookup textures like normal map or specular set this to false
	NOWORK_API static Texture2DPtr LoadHDR(const char* path);

	NOWORK_API void Update(const unsigned char* pixels);
	NOWORK_API void Update(const unsigned char* pixels, int width, int height);

	NOWORK_API void GenerateMipMaps();
protected:

	Texture2D();

	virtual void DoAsyncWork(int mode, void *params) override;

private:

	void LoadHDRInternal(const char* path);

	void CopyPixelData();

	GLubyte *m_Pixels;

	int m_InternalFormat;
	int m_Type;
	Texture::Format m_Format;
	bool m_Constant;
	bool m_srgb;
};