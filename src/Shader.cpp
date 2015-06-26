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


#include "Shader.h"
#include <fstream>
#include <streambuf>
#include "Mesh.h"

NOWORK_API Shader* Shader::DefaultUnlit;
NOWORK_API Shader* Shader::DefaultUnlitTextured;
NOWORK_API Shader* Shader::DefaultBlinPhong;

static bool ValidateShader(GLuint shader, const char* file = 0)
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

bool ValidateProgram(unsigned int program)
{
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;

	GLint loc = glGetUniformLocation(program, "ShadowTexture");
	if (loc != -1)
	{
		glUseProgram(program);
		glUniform1i(loc, 7);
		glUseProgram(0);
	}

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

NOWORK_API  Shader* Shader::Create(const std::string& vs, const std::string& fs)
{
	Shader* shader = new(std::nothrow) Shader();
	shader->m_VSObject = glCreateShader(GL_VERTEX_SHADER);
	shader->m_FSObject = glCreateShader(GL_FRAGMENT_SHADER);

	GLchar const* filesVS[]{vs.c_str()};
	GLchar const* filesFS[]{fs.c_str()};

	glShaderSource(shader->m_VSObject, 1, filesVS, 0);
	glShaderSource(shader->m_FSObject, 1, filesFS, 0);

	glCompileShader(shader->m_VSObject);
	if (!ValidateShader(shader->m_VSObject))
	{
		DelPtr(shader);
		return 0;
	}
	glCompileShader(shader->m_FSObject);
	if(!ValidateShader(shader->m_FSObject))
	{
		DelPtr(shader);
		return 0;
	}
	shader->m_ShaderObject = glCreateProgram();
	glAttachShader(shader->m_ShaderObject, shader->m_FSObject);
	glAttachShader(shader->m_ShaderObject, shader->m_VSObject);

	shader->BindAttributeLocation(0, "vertexPosition"); //bind input variables to their location ids
	shader->BindAttributeLocation(1, "vertexNormal");   //this is important so the variable ids match 
	shader->BindAttributeLocation(2, "vertexUV");       //the input ids from the mesh
	shader->BindAttributeLocation(3, "vertexColor");    //normally the driver sets them in order of definition but we cant be sure

	glLinkProgram(shader->m_ShaderObject);
	if(!ValidateProgram(shader->m_ShaderObject))
	{
		DelPtr(shader);
		return 0;
	}

	glBindAttribLocation(shader->m_ShaderObject, MODEL_VERTEX_LOCATION, "vertexPosition");
	glBindAttribLocation(shader->m_ShaderObject, MODEL_NORMAL_LOCATION, "vertexNormal");
	glBindAttribLocation(shader->m_ShaderObject, MODEL_TEXCOORD_LOCATION, "vertexUV");

	return shader;
}

NOWORK_API  Shader* Shader::Load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	LOG_DEBUG("Creating shader from file '" << vertexShaderPath << "' and '" << fragmentShaderPath << "'");

	std::ifstream t(vertexShaderPath, std::ios::in);
	if (!t.is_open())
	{
		LOG_ERROR("Unable to open file '" << vertexShaderPath << "': " << std::strerror(errno));
		return 0;
	}

	std::string vertexSrc((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
		
	t.close();

	t = std::ifstream(fragmentShaderPath, std::ios::in);
	if (!t.is_open())
	{
		LOG_ERROR("Unable to open file '" << vertexShaderPath << "': " << std::strerror(errno));
		return 0;
	}
	std::string fragmentSrc((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	t.close();

	return Create(vertexSrc, fragmentSrc);
}



void Shader::Use()
{
	glUseProgram(m_ShaderObject);
}

Shader::~Shader()
{
	glDetachShader(m_ShaderObject, m_VSObject);
	glDetachShader(m_ShaderObject, m_FSObject);

	glDeleteShader(m_FSObject);
	glDeleteShader(m_VSObject);
	glDeleteProgram(m_ShaderObject);
}

void Shader::InitializeDefaultShaders()
{
	const std::string defaultUnlitVertSrc =
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

	const std::string defaultUnlitFragSrc =
		"#version 130\n"
		"uniform vec4 DiffuseColor;\n"
		"\n"
		"in vec2 texCoord;\n"
		"\n"
		"out vec4 colorOut;\n"
		"\n"
		"void main( void )\n"
		"{\n"
		"    colorOut = vec4(1,1,1,1);\n"
		"}";

	const std::string defaultUnlitFragTexturedSrc =
		"#version 130\n"
		"uniform vec4 DiffuseColor;\n"
		"uniform sampler2D Texture;\n"
		"\n"
		"in vec2 texCoord;\n"
		"in vec3 vertColor;\n"
		"\n"
		"out vec4 colorOut;\n"
		"\n"
		"void main( void )\n"
		"{\n"
		"	 vec4 col = texture(Texture, texCoord);\n"
		"    if(col.a < 0.2)\n"
		"         discard;\n"
		"    colorOut = DiffuseColor * vec4(vertColor,1) * col;\n"
		"}";

	DefaultUnlit = Shader::Create(defaultUnlitVertSrc, defaultUnlitFragSrc);
	DefaultUnlitTextured = Shader::Create(defaultUnlitVertSrc, defaultUnlitFragTexturedSrc);
	DefaultBlinPhong = NULL; //TODO
}



/*******************************************************************************
Shader inline implementation:
*******************************************************************************/


 Shader::Shader()
{}

inline void Shader::SetParameterf(std::string name, float val)
{
	glUniform1f(GetAttributeLocation(name), val);
}
inline void Shader::SetParameteri(std::string name, int val)
{
	glUniform1i(GetAttributeLocation(name), val);
}
inline void Shader::SetParameterVec2(std::string name, glm::vec2 val)
{
	glUniform2f(GetAttributeLocation(name), val.x, val.y);
}
inline void Shader::SetParameterVec3(std::string name, glm::vec3 val)
{
	glUniform3f(GetAttributeLocation(name), val.x, val.y, val.z);
}

inline void Shader::SetParameterVec4(std::string name, glm::vec4 val)
{
	glUniform4f(GetAttributeLocation(name), val.x, val.y, val.z, val.w);
}

inline void Shader::SetParameterMat3(std::string name, glm::mat3 val)
{
	glUniformMatrix3fv(GetAttributeLocation(name), 1, GL_FALSE, glm::value_ptr(val));
}

inline void Shader::SetParameterMat4(std::string name, glm::mat4 val)
{
	glUniformMatrix4fv(GetAttributeLocation(name), 1, GL_FALSE, glm::value_ptr(val));
}

inline void Shader::SetDiffuseColor(glm::vec4 val)
{
	SetParameterVec4("DiffuseColor", val);
}

inline void Shader::SetDiffuseColor(float r, float g, float b, float a /*= 1*/)
{
	SetDiffuseColor(glm::vec4(r, g, b, a));
}

inline void Shader::SetParameterTexture(std::string name, Texture* tex, uint32_t slot)
{
	if (!tex) return;
	tex->Bind(slot);
	
	glUniform1i(GetAttributeLocation(name), slot);

}

void Shader::SetTexture(Texture* tex)
{
	SetParameterTexture("Texture", tex, 0);
}

NOWORK_API void Shader::BindAttributeLocation(unsigned int id, const std::string &name)
{
	glBindAttribLocation(m_ShaderObject, id, name.c_str());
}

int Shader::GetAttributeLocation(const std::string& name)
{
	auto res = m_ParamLocations.find(name);
	if (res != m_ParamLocations.end()) //key already requested?
		return res->second;

	//not requested yet
	GLint loc = glGetUniformLocation(m_ShaderObject, name.c_str());
	m_ParamLocations[name] = loc;
	return loc;
}
