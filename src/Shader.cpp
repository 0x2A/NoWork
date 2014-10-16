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
NOWORK_API Shader* Shader::DefaultBlinPhong;

static bool ValidateShader(GLuint shader, const char* file = 0)
{
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;

	glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
	if (length > 0) {
		LOG_ERROR("Shader " << shader << " (" << (file ? file : "") << ") compile error: " << buffer << std::endl);
		return false;
	}
	return true;
}

bool ValidateProgram(unsigned int program)
{
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;

	memset(buffer, 0, BUFFER_SIZE);
	glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
	if (length > 0)
	{
		LOG_ERROR("Program " << program << " link error: " << buffer << std::endl);
		return false;
	}
	
	glValidateProgram(program);
	GLint status;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
	{
		LOG_ERROR("Error validating shader " << program << std::endl);
		return false;
	}

	return true;
}

Shader* Shader::Create(std::string vs, std::string fs)
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
		delete shader;
		return 0;
	}
	glCompileShader(shader->m_FSObject);
	if(!ValidateShader(shader->m_FSObject))
	{
		delete shader;
		return 0;
	}
	shader->m_ShaderObject = glCreateProgram();
	glAttachShader(shader->m_ShaderObject, shader->m_FSObject);
	glAttachShader(shader->m_ShaderObject, shader->m_VSObject);
	glLinkProgram(shader->m_ShaderObject);
	if(!ValidateProgram(shader->m_ShaderObject))
	{
		delete shader;
		return 0;
	}

	glBindAttribLocation(shader->m_ShaderObject, MODEL_VERTEX_LOCATION, "vertexPosition");
	glBindAttribLocation(shader->m_ShaderObject, MODEL_NORMAL_LOCATION, "vertexNormal");
	glBindAttribLocation(shader->m_ShaderObject, MODEL_TEXCOORD_LOCATION, "vertexUV");

	return shader;
}

Shader* Shader::Load(std::string vertexShaderPath, std::string fragmentShaderPath)
{
	LOG_DEBUG("Creating shader from file '" << vertexShaderPath << "' and '" << fragmentShaderPath << "'");

	std::ifstream t(vertexShaderPath);
	if (!t.is_open())
	{
		LOG_ERROR("Unable to open file '" << vertexShaderPath << "'");
		return 0;
	}

	std::string vertexSrc((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	std::string fragmentSrc((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

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
		"#version 150\n"
		"in vec3 vertexPosition;\n"
		"in vec3 vertexNormal;\n"
		"in vec2 vertexUV;\n"
		"\n"
		"uniform mat4 MVPMatrix;\n"
		"uniform mat4 ModelViewMatrix;\n"
		"uniform mat4 ModelMatrix;\n"
		"\n"
		"out vec2 texCoord;\n"
		"\n"
		"void main( void )\n"
		"{\n"
		"    texCoord = vertexUV;\n"
		"    gl_Position = MVPMatrix * vec4(vertexPosition,1);\n"
		"}";

	const std::string defaultUnlitFragSrc =
		"#version 150\n"
		"uniform vec4 DiffuseColor;\n"
		"\n"
		"in vec2 texCoord;\n"
		"\n"
		"out vec4 colorOut;\n"
		"\n"
		"void main( void )\n"
		"{\n"
		"    colorOut = DiffuseColor;\n"
		"}";

	DefaultUnlit = Shader::Create(defaultUnlitVertSrc, defaultUnlitFragSrc);
	DefaultBlinPhong = NULL; //TODO
}



/*******************************************************************************
Shader inline implementation:
*******************************************************************************/


 Shader::Shader()
{}

inline void Shader::SetParameterf(std::string name, float val)
{
	glUniform1f(glGetUniformLocation(m_ShaderObject, name.c_str()), val);
}
inline void Shader::SetParameteri(std::string name, int val)
{
	glUniform1i(glGetUniformLocation(m_ShaderObject, name.c_str()), val);
}
inline void Shader::SetParameterVec2(std::string name, glm::vec2 val)
{
	glUniform2f(glGetUniformLocation(m_ShaderObject, name.c_str()), val.x, val.y);
}
inline void Shader::SetParameterVec3(std::string name, glm::vec3 val)
{
	glUniform3f(glGetUniformLocation(m_ShaderObject, name.c_str()), val.x, val.y, val.z);
}

inline void Shader::SetParameterVec4(std::string name, glm::vec4 val)
{
	glUniform4f(glGetUniformLocation(m_ShaderObject, name.c_str()), val.x, val.y, val.z, val.w);
}

inline void Shader::SetParameterMat3(std::string name, glm::mat3 val)
{
	glUniformMatrix3fv(glGetUniformLocation(m_ShaderObject, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

inline void Shader::SetParameterMat4(std::string name, glm::mat4 val)
{
	glUniformMatrix4fv(glGetUniformLocation(m_ShaderObject, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

inline void Shader::SetDiffuseColor(glm::vec4 val)
{
	SetParameterVec4("DiffuseColor", val);
}

inline void Shader::SetDiffuseColor(float r, float g, float b, float a /*= 1*/)
{
	SetDiffuseColor(glm::vec4(r, g, b, a));
}
