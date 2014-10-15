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

}