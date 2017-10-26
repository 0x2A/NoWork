#pragma once

#include "nowork/Common.h"
#include "NoWork/Shader.h"
#include "NoWork/Texture2D.h"
#include "NoWork/Mesh.h"


class Font;
typedef std::shared_ptr<Font> FontPtr;

namespace ftgl
{
	struct texture_atlas_t;
	struct texture_font_t;
	struct vertex_buffer_t;
}

class Font
{
	friend class NoWork;

public:
	struct CacheData
	{
		int bitmap_left;
		int bitmap_top;
		unsigned char* buffer;
		int width;
		int rows;
		glm::ivec2 advance;

		CacheData()
			: bitmap_left(0), bitmap_top(0), width(0), rows(0), buffer(0)
		{}

		CacheData(int l, int t, int w, int r, glm::ivec2 a)
			: bitmap_left(l), bitmap_top(t), width(w), rows(r), advance(a)
		{
		}
	};

	NOWORK_API Font();
	NOWORK_API ~Font();

	NOWORK_API static FontPtr Create(const char* path, float size = 24.0f);

	NOWORK_API void SetSize(int s);
	NOWORK_API int GetSize() { return m_Size; }

	NOWORK_API void PrintText(const char* t, glm::vec2 pos, glm::vec4 color = glm::vec4(1,1,1,1), glm::vec2 scale = glm::vec2(1, 1));
	NOWORK_API void PrintShadowedText(const char* t, glm::vec2 pos, glm::vec4 color = glm::vec4(1, 1, 1, 1), glm::vec2 scale = glm::vec2(1, 1), glm::vec2 shadowOffset = glm::vec2(1,1));
	NOWORK_API void Render();

	NOWORK_API float TextWidth(const char* t);

protected:
	static bool Init(NoWork* framework);
	static void Shutdown();

private:


	NOWORK_API static NoWork* m_Framework;

	ftgl::texture_atlas_t *m_Atlas;
	ftgl::texture_font_t *m_Font;
	ftgl::vertex_buffer_t *m_Buffer;
	ShaderPtr m_Shader;

	/* Texture atlas to store individual glyphs */

	int m_Size;
};