#include "nowork/SpriteSheet.h"
#include "nowork/Log.h"
#include <fstream>
#include "nowork/Texture2D.h"



MeshPtr SpriteSheet::m_PlaneMesh;
ShaderPtr SpriteSheet::m_SpriteShader;
ShaderPtr SpriteSheet::m_SpriteKeyedShader;

const std::string spriteShaderVert = 
		"#version 130\n"
		"in vec3 vertexPosition;\n"
		"in vec3 vertexNormal;\n"
		"in vec2 vertexUV;\n"
		"in vec3 vertexColor;\n"
		"\n"
		"uniform mat4 MVPMatrix;\n"
		"uniform mat4 ModelViewMatrix;\n"
		"uniform mat4 ModelMatrix;\n"
		"\n"
		"out vec2 texCoord;\n"
		"out vec3 vertColor;\n"
		"\n"
		"void main( void )\n"
		"{\n"
		"    texCoord = vertexUV;\n"
		"    vertColor = vertexColor;\n"
		"    gl_Position = MVPMatrix * vec4(vertexPosition,1);\n"
		"}";

const std::string spriteShaderFrag =
		"#version 130\n"
		"uniform vec4 DiffuseColor;\n"
		"uniform sampler2D Texture;\n"
		"uniform vec4 texCoordParams;\n"
		"\n"
		"in vec2 texCoord;\n"
		"in vec3 vertColor;\n"
		"\n"
		"out vec4 colorOut;\n"
		"\n"
		"void main( void )\n"
		"{\n"
		"	 vec4 col = texture(Texture, texCoord * texCoordParams.zw + texCoordParams.xy);\n"
		"    if(col.a < 0.15)\n"
		"         discard;\n"
		"    colorOut = DiffuseColor * vec4(vertColor,1) * col;\n"
		"}";

const std::string spriteShaderKeyedFrag =
		"#version 130\n"
		"uniform vec4 DiffuseColor;\n"
		"uniform sampler2D Texture;\n"
		"uniform vec4 texCoordParams;\n"
		"uniform vec3 colorKey;\n"
		"uniform vec2 colorKeyParams;\n"
		"\n"
		"in vec2 texCoord;\n"
		"in vec3 vertColor;\n"
		"\n"
		"out vec4 colorOut;\n"
		"\n"
		"void main( void )\n"
		"{\n"
		"	 vec4 col = texture(Texture, texCoord * texCoordParams.zw + texCoordParams.xy);\n"
		"float maskY = 0.2989 * colorKey.r + 0.5866 * colorKey.g + 0.1145 * colorKey.b;\n"
		"float maskCr = 0.7132 * (colorKey.r - maskY);\n"
		"float maskCb = 0.5647 * (colorKey.b - maskY);\n"

		"float Y = 0.2989 * col.r + 0.5866 * col.g + 0.1145 * col.b;\n"
		"float Cr = 0.7132 * (col.r - Y);\n"
		"float Cb = 0.5647 * (col.b - Y);\n"

		"float blendValue = smoothstep(colorKeyParams.x, colorKeyParams.x + colorKeyParams.y, distance(vec2(Cr, Cb), vec2(maskCr, maskCb)));\n"
		
		"    if(blendValue < 0.15)\n"
		"         discard;\n"
		"vec4 finalCol = DiffuseColor * vec4(vertColor,1) * col * blendValue;"
		"    colorOut = finalCol;\n"
		"}";

SpriteSheetPtr SpriteSheet::Load(const std::string& path)
{
	
	LOG_DEBUG("Loading SpriteSheet from file '" << path << "'");

	//load file content to string
	std::ifstream t(path, std::ios::in);
	if (!t.is_open())
	{
		LOG_ERROR("Unable to open file '" << path << "': " << std::strerror(errno));
		return nullptr;
	}

	std::string jsonSrc((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	t.close();

	//try parsing json file
	rapidjson::Document docRoot;
	if (docRoot.Parse<0>(jsonSrc.c_str()).HasParseError()) //is there any error while parsing the json file
	{
		LOG_ERROR("Loading SpriteSheet '" << path << "':\n" << docRoot.GetParseError() << " at line " << docRoot.GetErrorLine());
		return nullptr;
	}
	
	//is value name defined?
	if (!GotValue(docRoot, "SpriteSheet", "Not a vaild SpriteSheet description file. Must have member \"SpriteSheet\"",false, true))
	{
		return nullptr;
	}
	rapidjson::Value& doc = docRoot["SpriteSheet"];
	
	SpriteSheetPtr sheet = std::make_shared<SpriteSheet>();


	//load the texture
	if (!GotValue(doc, "texture", "No texture path defined", false, true))
	{
		return nullptr;
	}

	Texture2DPtr spriteSheetTex = Texture2D::Load(doc["texture"].GetString());
	if (!spriteSheetTex) //is the texture loaded?
	{
		return nullptr;
	}

	//load default data
	if (!m_PlaneMesh)
		m_PlaneMesh = Mesh::CreatePlane();

	if (!m_SpriteShader)
		m_SpriteShader = Shader::Create(spriteShaderVert, spriteShaderFrag);
	
	if (!m_SpriteKeyedShader)
		m_SpriteKeyedShader = Shader::Create(spriteShaderVert, spriteShaderKeyedFrag);

	sheet->m_SpriteTexture = spriteSheetTex;

	//do we have any sprites defined?
	if (GotValue(doc, "sprites", "", false, false))
	{
		if (!ParseSprites(sheet, doc["sprites"]))
		{
			//something got wrong
			return nullptr;
		}
	}
	if (GotValue(doc, "animations", "", false, false))
	{
		if (!ParseAnimations(sheet, doc["animations"]))
		{
			//something got wrong
			return nullptr;
		}
	}

	return sheet;
}

bool SpriteSheet::GetArea(rapidjson::Value& doc, Area<int>* targetArea)
{
	if (!GotValue(doc, "posx", "posx not defined for sprite element", false, true))
		return false;
	int posX = doc["posx"].GetInt();

	if (!GotValue(doc, "posy", "posy not defined for sprite element", false, true))
		return false;
	int posY = doc["posy"].GetInt();

	if (!GotValue(doc, "width", "width not defined for sprite element", false, true))
		return false;
	int width = doc["width"].GetInt();

	if (!GotValue(doc, "height", "height not defined for sprite element", false, true))
		return false;
	int height = doc["height"].GetInt();

	*targetArea = Area<int>(posX, posY, posX + width, posY + height);
	return true;
}

bool SpriteSheet::ParseSprites(const SpriteSheetPtr& sheet, rapidjson::Value& doc)
{
	int numSprites = doc.Size();
	sheet->m_Sprites.reserve(numSprites);

	for (int i = 0; i < numSprites; i++)
	{
		Area<int> area;
		if (!GetArea(doc[i], &area))
			continue;

		SpritePtr sprite = SpritePtr(new Sprite(area, sheet->m_SpriteTexture));
		sheet->m_Sprites.push_back(sprite);
	}
	
	return true;
}

bool SpriteSheet::ParseAnimations(const SpriteSheetPtr& sheet, rapidjson::Value& doc)
{
	int numAnimations = doc.Size();

	for (int i = 0; i < numAnimations; i++)
	{
		std::string str = "no frames defined for animation " + std::to_string(i);
		if (!GotValue(doc[i], "frames", str, false, true))
			continue;

		std::string name = "";
		int frameRate = 15;
		if (GotValue(doc[i], "name", "no name defined for animation", true, false))
			name = doc[i]["name"].GetString();

		if (GotValue(doc[i], "framerate", "no framerate defined for animation. setting to default 15 fps.", true, false))
			frameRate = doc[i]["framerate"].GetInt();

		std::vector<Area<int>> frames;
		rapidjson::Value& frameEntries = doc[i]["frames"];
		frames.reserve(frameEntries.Size());
		for (int j = 0; j < frameEntries.Size(); j++)
		{
			Area<int> area;
			if (!GetArea(frameEntries[j], &area))
				continue;

			frames.push_back(area);
		}

		SpriteAnimationPtr animation = std::make_shared<SpriteAnimation>(frames, frameRate, name, sheet->m_SpriteTexture);
		sheet->m_SpriteAnimations.push_back(animation);
	}

	return true;
}


bool SpriteSheet::GotValue(rapidjson::Value& doc, const char* name, const std::string& msg, bool warning /*= false*/, bool error /*= true*/)
{
	if (!doc.HasMember(name))
	{
		if (warning) LOG_WARNING("Loading SpriteSheet: " << msg)
		else if(error) LOG_ERROR("Loading SpriteSheet: " << msg);

		return false;
	}
	return true;
}

SpriteSheet::~SpriteSheet()
{
	m_Sprites.clear();
	m_SpriteAnimations.clear();
}

SpritePtr SpriteSheet::GetSprite(size_t index)
{
	if (index >= m_Sprites.size())
		return nullptr;

	return m_Sprites[index];
}

SpriteAnimationPtr SpriteSheet::GetAnimation(size_t index)
{
	if (index >= m_SpriteAnimations.size())
		return nullptr;

	return m_SpriteAnimations[index];
}

void SpriteSheet::SetLinearFiltering(bool b)
{
	m_SpriteTexture->SetLinearTextureFilter(b);
}

void SpriteSheet::SetColorKey(glm::vec3 color)
{
	for (auto sprite : m_Sprites)
	{
		sprite->SetColorKey(color);
	}
	for (auto sprite : m_SpriteAnimations)
	{
		sprite->SetColorKey(color);
	}
}

void SpriteSheet::DisableColorKey()
{
	for (auto sprite : m_Sprites)
	{
		sprite->DisableColorKey();
	}
	for (auto sprite : m_SpriteAnimations)
	{
		sprite->DisableColorKey();
	}
}


