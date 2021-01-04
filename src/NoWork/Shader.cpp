/*
*  ===========================================================================
*	This file is part of CrystalEngine source code.
*   Copyright (C) 2013  Frank Köhnke
*
*   This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*  ===========================================================================
*/


#include "nowork/Shader.h"

#include "nowork/Mesh.h"
#include "nowork/Framework.h"
#include "NoWork/FileSystem.h"

#include <regex>

NOWORK_API ShaderPtr Shader::DefaultUnlit;
NOWORK_API ShaderPtr Shader::DefaultUnlitTextured;
NOWORK_API ShaderPtr Shader::DefaultBlinPhong;
NOWORK_API ShaderPtr Shader::ScreenAlignedTextured;
NOWORK_API ShaderPtr Shader::ScreenAligned;
NOWORK_API ShaderPtr Shader::BlitScreenShader;


bool Shader::ValidateShader(GLuint shader, const char* file)
{
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;
	GLint success;

	//Note 1: Changed Validation: Intel returns 'No Errors' as InfoLog when in debug context
	//so we cant simply check for InfoLog to verify the successfull compilation of the shader.
	//So we first have to check the compile status and then get the detailed message.
	//that way we cant missinterpret compile information as error message (thx intel...)
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE)
	{
		glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
		if (length > 0) {
			LOG_ERROR("Shader " << shader << " (" << (file ? file : "") << ") compile error: " << buffer << std::endl);
			return false;
		}
	}
	return true;
}

bool Shader::ValidateProgram(unsigned int program)
{
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;


	glValidateProgram(program);
	GLint status;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
	{
		LOG_ERROR("Error validating shader " << program << std::endl);

		glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
		if (length > 0)
		{
			LOG_ERROR("Program " << program << " link error: " << buffer << std::endl);
		}
		//return false;
	}


	return true;
}

NOWORK_API  ShaderPtr Shader::Create(const char* name, const char* vs, const char* fs)
{
	ShaderPtr shader = ShaderPtr(new Shader);
	shader->m_Name = name;

	if (!NoWork::IsMainThread())
	{
		std::string* arr = new std::string[2];
		arr[0] = vs;
		arr[1] = fs;
		shader->AddToGLQueue(0,arr);
		return shader;
	}

	if (!shader->CompileShaders(vs, fs))
	{
		return nullptr;
	}

	return shader;
}



NOWORK_API  ShaderPtr Shader::Load(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	LOG_DEBUG("Creating shader from file '" << vertexShaderPath << "' and '" << fragmentShaderPath << "'");

	std::string vertexSrc = FileSystem::LoadTextFile(vertexShaderPath);
	vertexSrc = PreprocessIncludes(vertexSrc, FileSystem::GetFilename(vertexShaderPath), FileSystem::GetPath(vertexShaderPath));
	
	std::string fragmentSrc = FileSystem::LoadTextFile(fragmentShaderPath);
	fragmentSrc = PreprocessIncludes(fragmentSrc, FileSystem::GetFilename(fragmentShaderPath), FileSystem::GetPath(fragmentShaderPath));

	// Remove directory if present.
	// Do this before extension removal incase directory has a period character.

	std::string filename = vertexShaderPath;

	const size_t last_slash_idx = filename.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
	{
		filename.erase(0, last_slash_idx + 1);
	}

	// Remove extension if present.
	const size_t period_idx = filename.rfind('.');
	if (std::string::npos != period_idx)
	{
		filename.erase(period_idx);
	}

	return Create(filename.c_str(), vertexSrc.c_str(), fragmentSrc.c_str());
}



void Shader::Use()
{
	glUseProgram(m_ShaderObject);
}

Shader::~Shader()
{
	if(m_VSObject != 0)
		glDetachShader(m_ShaderObject, m_VSObject);
	if(m_FSObject != 0)
		glDetachShader(m_ShaderObject, m_FSObject);

	if(m_FSObject != 0)
		glDeleteShader(m_FSObject);
	if(m_FSObject != 0)
		glDeleteShader(m_VSObject);

	glDeleteProgram(m_ShaderObject);
}

void Shader::InitializeDefaultShaders()
{
	const char* defaultUnlitVertSrc =
#include "Shaders/defaultUnlit.vs"
		;

	const char* defaultUnlitFragSrc =
#include "Shaders/defaultUnlit.fs"
		;

	const char* defaultUnlitFragTexturedSrc =
#include "Shaders/defaultUnlitTextured.fs"		
		;

	const char* defaultBlinPhongFragSrc = 
#include "Shaders/defaultBlinPhong.fs"
		;
	const char* screenAlignedVertSrc =
#include "Shaders/screenAligned.vs"
		;

	const char* BlitScreenShaderVertSrc = 
#include "Shaders/blit.vs"
		;
	const char* BlitScreenShaderFragSrc = 
#include "Shaders/blit.fs"
		;

	DefaultUnlit = Shader::Create("defaultUnlit", defaultUnlitVertSrc, defaultUnlitFragSrc);
	DefaultUnlitTextured = Shader::Create("defaultUnlitTextured", defaultUnlitVertSrc, defaultUnlitFragTexturedSrc);
	DefaultBlinPhong = Shader::Create("defaultBlinPhong", defaultUnlitVertSrc, defaultBlinPhongFragSrc);
	DefaultBlinPhong->Use();
	DefaultBlinPhong->SetParameterVec3("LightPos", glm::vec3(2,1,0));

	ScreenAligned = Shader::Create("ScreenAligned", screenAlignedVertSrc, defaultUnlitFragSrc);
	ScreenAlignedTextured = Shader::Create("screenAlignedTextured", screenAlignedVertSrc, defaultUnlitFragTexturedSrc);
	BlitScreenShader = Shader::Create("BlitScreenShader", BlitScreenShaderVertSrc, BlitScreenShaderFragSrc);

}



/*******************************************************************************
Shader inline implementation:
*******************************************************************************/


 Shader::Shader()
{
	 m_VSObject = 0;
	 m_FSObject = 0;
 }

void Shader::SetParameterf(const char* name, float val)
{
	glUniform1f(GetAttributeLocation(name), val);
}

NOWORK_API void Shader::SetParameterfv(const char* name, int num, float *val)
{
	glUniform1fv(GetAttributeLocation(name), num, val);
}

void Shader::SetParameteri(const char* name, int val)
{
	glUniform1i(GetAttributeLocation(name), val);
}
void Shader::SetParameterVec2(const char* name, glm::vec2 val)
{
	glUniform2f(GetAttributeLocation(name), val.x, val.y);
}
void Shader::SetParameterVec3(const char* name, glm::vec3 val)
{
	glUniform3f(GetAttributeLocation(name), val.x, val.y, val.z);
}

void Shader::SetParameterVec4(const char* name, glm::vec4 val)
{
	glUniform4f(GetAttributeLocation(name), val.x, val.y, val.z, val.w);
}

void Shader::SetParameterMat3(const char* name, glm::mat3 val)
{
	glUniformMatrix3fv(GetAttributeLocation(name), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::SetParameterMat4(const char* name, glm::mat4 val)
{
	glUniformMatrix4fv(GetAttributeLocation(name), 1, GL_FALSE, glm::value_ptr(val));
}
NOWORK_API void Shader::SetParameterMat4v(const char* name, int num, glm::mat4* val)
{
	glUniformMatrix4fv(GetAttributeLocation(name), num, GL_FALSE, glm::value_ptr(val[0]));
}

void Shader::SetDiffuseColor(glm::vec4 val)
{
	SetParameterVec4("DiffuseColor", val);
}

void Shader::SetDiffuseColor(float r, float g, float b, float a /*= 1*/)
{
	SetDiffuseColor(glm::vec4(r, g, b, a));
}

void Shader::SetParameterTexture(const char* name, TexturePtr tex, uint32_t slot)
{
	if (!tex) return;
	tex->Bind(slot);
	
	glUniform1i(GetAttributeLocation(name), slot);

}

void Shader::SetParameterTexturev(const char* name, int num, TexturePtr *tex, uint32_t *slots)
{
	if (!tex) return;
	for (int i = 0; i < num; i++)
		tex[i]->Bind(slots[i]);

	glUniform1iv(GetAttributeLocation(name), num, (GLint*)slots);
}

void Shader::SetParameterTexturev(const char* name, int num, RenderTexturePtr *tex, uint32_t *slots)
{
	if (!tex) return;
	for (int i = 0; i < num; i++)
		tex[i]->Bind(slots[i]);

	glUniform1iv(GetAttributeLocation(name), num, (GLint*)slots);
}

void Shader::SetTexture(TexturePtr tex)
{
	SetParameterTexture("Texture", tex, 0);
}

NOWORK_API void Shader::BindAttributeLocation(unsigned int id, const char* name)
{
	glBindAttribLocation(m_ShaderObject, id, name);
}

int Shader::GetAttributeLocation(const char* name)
{
	auto res = m_ParamLocations.find(name);
	if (res != m_ParamLocations.end()) //key already requested?
		return res->second;

	//not requested yet
	GLint loc = glGetUniformLocation(m_ShaderObject, name);
	m_ParamLocations[name] = loc;

	if (loc < 0)
	{
		LOG_WARNING("Shader '" + m_Name + "' has no uniform attribute ' " + name + "'");
	}
	return loc;
}

void Shader::DoAsyncWork(int mode, void *params)
{
	std::string* arr = static_cast<std::string*>(params);
	CompileShaders(arr[0], arr[1]);
}

bool Shader::CompileShaders(const std::string& vs, const std::string& fs)
{
	m_VSObject = glCreateShader(GL_VERTEX_SHADER);
	m_FSObject = glCreateShader(GL_FRAGMENT_SHADER);

	GLchar const* filesVS[]{vs.c_str()};
	GLchar const* filesFS[]{fs.c_str()};

	glShaderSource(m_VSObject, 1, filesVS, 0);
	glShaderSource(m_FSObject, 1, filesFS, 0);

	glCompileShader(m_VSObject);
	if (!ValidateShader(m_VSObject))
	{
		return false;
	}
	glCompileShader(m_FSObject);
	if (!ValidateShader(m_FSObject))
	{
		return false;
	}
	m_ShaderObject = glCreateProgram();
	glAttachShader(m_ShaderObject, m_FSObject);
	glAttachShader(m_ShaderObject, m_VSObject);

	BindAttributeLocation(0, "vertexPosition"); //bind input variables to their location ids
	BindAttributeLocation(1, "vertexNormal");   //this is important so the variable ids match 
	BindAttributeLocation(2, "vertexUV");       //the input ids from the mesh
	BindAttributeLocation(3, "vertexColor");    //normally the driver sets them in order of definition but we cant be sure

	glLinkProgram(m_ShaderObject);
	if (!ValidateProgram(m_ShaderObject))
	{
		return false;
	}

	glBindAttribLocation(m_ShaderObject, MODEL_VERTEX_LOCATION, "vertexPosition");
	glBindAttribLocation(m_ShaderObject, MODEL_NORMAL_LOCATION, "vertexNormal");
	glBindAttribLocation(m_ShaderObject, MODEL_TEXCOORD_LOCATION, "vertexUV");

	return true;
}

std::string Shader::PreprocessIncludes(const std::string &source, const std::string &filename, const std::string& path, int level /*= 0*/)
{
	if (level > 32)
	{
		LOG_ERROR("header inclusion depth limit reached, might be caused by cyclic header inclusion. Caused in " << path);
		return source;
	}
	static const std::regex re("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
	std::stringstream input;
	std::stringstream output;
	input << source;

	size_t line_number = 1;
	std::smatch matches;

	std::string line;
	while (std::getline(input, line))
	{
		bool dontPrintLine = false;
		if (level == 0 && line_number == 1) { //dont print #line at beginning of shader code, so we dont get trouble with #version strings and stuff
			dontPrintLine = true;
		}
		if (std::regex_search(line, matches, re))
		{
			std::string include_file = matches[1];
			std::string include_string;

			try
			{
				std::string tpath(path);
				tpath += "/" + include_file;
				include_string = FileSystem::LoadTextFile(tpath.c_str());
			}
			catch (std::exception &e)
			{
				LOG_ERROR(filename << "(" << line_number << ") : fatal error: cannot open include file " << include_file);
				return "";
			}
			output << PreprocessIncludes(include_string, include_file, path, level + 1) << std::endl;
		}
		else
		{
			if (!dontPrintLine) output << "#line " << line_number << " \"" << filename << "\"" << std::endl;
			output << line << std::endl;
		}
		++line_number;
	}
	return output.str();
}

