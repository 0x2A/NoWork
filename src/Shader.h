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
#include "Texture.h"

class Shader
{
	friend class NoWork;
public:

	NOWORK_API virtual ~Shader();
	NOWORK_API static Shader* Create(const std::string& vs, const std::string& fs);
	NOWORK_API static Shader* Load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

	NOWORK_API void Use();

	NOWORK_API void Reload();


	NOWORK_API void SetParameterf(std::string name, float val);
	NOWORK_API void SetParameteri(std::string name, int val);
	NOWORK_API void SetParameterVec2(std::string name, glm::vec2 val);
	NOWORK_API void SetParameterVec3(std::string name, glm::vec3 val);
	NOWORK_API void SetParameterVec4(std::string name, glm::vec4 val);
	NOWORK_API void SetParameterMat3(std::string name, glm::mat3 val);
	NOWORK_API void SetParameterMat4(std::string name, glm::mat4 val);
	NOWORK_API void SetParameterTexture(std::string name, Texture* tex, uint32_t slot);

	NOWORK_API void BindAttributeLocation(unsigned int id, const std::string &name);
	NOWORK_API int GetAttributeLocation(const std::string& name);

	//Sets a texture to uniform 'texture'. Useful for default shaders.
	//Note: if you want to use more than one texture use SetParameterTexture
	NOWORK_API void SetTexture(Texture* tex);

	NOWORK_API void SetDiffuseColor(glm::vec4 val);
	NOWORK_API void SetDiffuseColor(float r, float g, float b, float a = 1);

	NOWORK_API static Shader* DefaultUnlit;
	NOWORK_API static Shader* DefaultUnlitTextured;
	NOWORK_API static Shader* DefaultBlinPhong;

protected:

	NOWORK_API Shader();

	static void InitializeDefaultShaders();

private:
	std::string m_VertexSrc;
	std::string m_FragmentSrc;

	unsigned int m_ShaderObject;
	unsigned int m_VSObject;
	unsigned int m_FSObject;
	
	std::map<std::string, int> m_ParamLocations;
};
