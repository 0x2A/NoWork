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

#pragma once

#include "Common.h"
#include "Log.h"

class Shader
{
	friend class NoWork;
public:
	
	NOWORK_API static Shader* Create(std::string vs, std::string fs);
	NOWORK_API static Shader* Load(std::string vertexShaderPath, std::string fragmentShaderPath);

	NOWORK_API void Use();

	NOWORK_API void Reload();


	NOWORK_API void SetParameterf(std::string name, float val, int progIndex = 0);
	NOWORK_API void SetParameteri(std::string name, int val, int progIndex = 0);
	NOWORK_API void SetParameterVec2(std::string name, glm::vec2 val, int progIndex = 0);
	NOWORK_API void SetParameterVec3(std::string name, glm::vec3 val, int progIndex = 0);
	NOWORK_API void SetParameterVec4(std::string name, glm::vec4 val, int progIndex = 0);
	NOWORK_API void SetParameterMat3(std::string name, glm::mat3 val, int progIndex = 0);
	NOWORK_API void SetParameterMat4(std::string name, glm::mat4 val, int progIndex = 0);

	NOWORK_API static Shader* DefaultUnlit;
	NOWORK_API static Shader* DefaultBlinPhong;

protected:

	NOWORK_API Shader();
	NOWORK_API virtual ~Shader();

	static void InitializeDefaultShaders();

private:
	std::string m_VertexSrc;
	std::string m_FragmentSrc;

	unsigned int m_ShaderObject;
	unsigned int m_VSObject;
	unsigned int m_FSObject;
};


/*******************************************************************************
	Shader inline implementation:
*******************************************************************************/


inline Shader::Shader()
{}

inline void Shader::SetParameterf(std::string name, float val, int progIndex)
{
	glUniform1f(glGetUniformLocation(m_ShaderObject, name.c_str()), val);
}
inline void Shader::SetParameteri(std::string name, int val, int progIndex)
{
	glUniform1i(glGetUniformLocation(m_ShaderObject, name.c_str()), val);
}
inline void Shader::SetParameterVec2(std::string name, glm::vec2 val, int progIndex)
{
	glUniform2f(glGetUniformLocation(m_ShaderObject, name.c_str()), val.x, val.y);
}
inline void Shader::SetParameterVec3(std::string name, glm::vec3 val, int progIndex)
{
	glUniform3f(glGetUniformLocation(m_ShaderObject, name.c_str()), val.x, val.y, val.z);
}

inline void Shader::SetParameterVec4(std::string name, glm::vec4 val, int progIndex)
{
	glUniform4f(glGetUniformLocation(m_ShaderObject, name.c_str()), val.x, val.y, val.z, val.w);
}

inline void Shader::SetParameterMat3(std::string name, glm::mat3 val, int progIndex)
{
	glUniformMatrix3fv(glGetUniformLocation(m_ShaderObject, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

inline void Shader::SetParameterMat4(std::string name, glm::mat4 val, int progIndex)
{
	glUniformMatrix4fv(glGetUniformLocation(m_ShaderObject, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}
static bool ValidateProgram(unsigned int program);