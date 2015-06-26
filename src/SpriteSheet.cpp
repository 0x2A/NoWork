#include "SpriteSheet.h"
#include "Log.h"
#include <fstream>
#include "Texture2D.h"



Mesh * SpriteSheet::m_PlaneMesh = 0;
Shader* SpriteSheet::m_SpriteShader = 0;
Shader* SpriteSheet::m_SpriteKeyedShader = 0;

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

SpriteSheet* SpriteSheet::Load(const std::string& path)
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
	
	SpriteSheet *sheet = new SpriteSheet;

	//try to get the spriteSheet name
	if (GotValue(doc, "name", "Name not defined",true, false))
		sheet->m_Name = doc["name"].GetString();

	//load the texture
	if (!GotValue(doc, "texture", "No texture path defined", false, true))
	{
		DelPtr(sheet);
		return nullptr;
	}

	Texture2D *spriteSheetTex = Texture2D::Load(doc["texture"].GetString());
	if (!spriteSheetTex) //is the texture loaded?
	{
		DelPtr(sheet);
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
			DelPtr(sheet);
			return nullptr;
		}
	}
	if (GotValue(doc, "animations", "", false, false))
	{
		if (!ParseAnimations(sheet, doc["animations"]))
		{
			//something got wrong
			DelPtr(sheet);
			return nullptr;
		}
	}

	return sheet;
}

bool SpriteSheet::ParseSprites(SpriteSheet* sheet, rapidjson::Value& doc)
{
	int numSprites = doc.Size();
	sheet->m_Sprites.reserve(numSprites);

	for (int i = 0; i < numSprites; i++)
	{
		if (!GotValue(doc[i], "posx", "posx not defined for sprite element", false, true))
			return false;
		int posX = doc[i]["posx"].GetInt(); 

		if (!GotValue(doc[i], "posy", "posy not defined for sprite element", false, true))
			return false;
		int posY = doc[i]["posy"].GetInt();

		if (!GotValue(doc[i], "width", "width not defined for sprite element", false, true))
			return false;
		int width = doc[i]["width"].GetInt();

		if (!GotValue(doc[i], "height", "height not defined for sprite element", false, true))
			return false;
		int height = doc[i]["height"].GetInt();

		Sprite *sprite = new Sprite(Area<int>(posX, posY, posX + width, posY + height), sheet->m_SpriteTexture);
		sheet->m_Sprites.push_back(sprite);
	}
	
	return true;
}

bool SpriteSheet::ParseAnimations(SpriteSheet* sheet, rapidjson::Value& doc)
{
	//TODO
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
	DelPtr(m_SpriteTexture);

	for (auto sprite : m_Sprites)
	{
		delete sprite;
	}
	m_Sprites.clear();
}

Sprite* SpriteSheet::GetSprite(size_t index)
{
	if (index >= m_Sprites.size())
		return nullptr;

	return m_Sprites[index];
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
}

void SpriteSheet::DisableColorKey()
{
	for (auto sprite : m_Sprites)
	{
		sprite->DisableColorKey();
	}
}

