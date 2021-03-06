/*
*  ===========================================================================
*	This file is part of CrystalEngine source code.
*   Copyright (C) 2013  Frank K�hnke
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

#include "nowork/Common.h"
#include "nowork/Log.h"
#include "nowork/Texture.h"
#include "nowork/RenderTexture.h"
#include "AsyncGLWorker.h"


#define SHADER_SRC(x) #x

class Shader;
typedef std::shared_ptr<Shader> ShaderPtr;

class Shader : public AsyncGLWorker
{
	friend class NoWork;
public:

	NOWORK_API virtual ~Shader();
	NOWORK_API static ShaderPtr Create(const char* name, const char* vs, const char* fs);
	NOWORK_API static ShaderPtr Load(const char* vertexShaderPath, const char* fragmentShaderPath);

	NOWORK_API void Use();

	NOWORK_API void Reload();


	NOWORK_API void SetParameterf(const char* name, float val);
	NOWORK_API void SetParameterfv(const char* name, int num, float *val);
	NOWORK_API void SetParameteri(const char* name, int val);
	NOWORK_API void SetParameterVec2(const char* name, glm::vec2 val);
	NOWORK_API void SetParameterVec3(const char* name, glm::vec3 val);
	NOWORK_API void SetParameterVec4(const char* name, glm::vec4 val);
	NOWORK_API void SetParameterMat3(const char* name, glm::mat3 val);
	NOWORK_API void SetParameterMat4(const char* name, glm::mat4 val);
	NOWORK_API void SetParameterMat4v(const char* name, int num, glm::mat4* val);
	NOWORK_API void SetParameterTexture(const char* name, TexturePtr tex, uint32_t slot);
	NOWORK_API void SetParameterTexturev(const char* name, int num, TexturePtr *tex, uint32_t *slots);
	NOWORK_API void SetParameterTexturev(const char* name, int num, RenderTexturePtr *tex, uint32_t *slots);

	NOWORK_API void BindAttributeLocation(unsigned int id, const char* name);
	NOWORK_API int GetAttributeLocation(const char* name);

	//Sets a texture to uniform 'texture'. Useful for default shaders.
	//Note: if you want to use more than one texture use SetParameterTexture
	NOWORK_API void SetTexture(TexturePtr tex);

	NOWORK_API void SetDiffuseColor(glm::vec4 val);
	NOWORK_API void SetDiffuseColor(float r, float g, float b, float a = 1);

	NOWORK_API static ShaderPtr DefaultUnlit;
	NOWORK_API static ShaderPtr DefaultUnlitTextured;
	NOWORK_API static ShaderPtr DefaultBlinPhong;
	NOWORK_API static ShaderPtr ScreenAligned;
	NOWORK_API static ShaderPtr ScreenAlignedTextured;
	NOWORK_API static ShaderPtr BlitScreenShader;

protected:

	NOWORK_API Shader();

	static void InitializeDefaultShaders();

	static std::string PreprocessIncludes(const std::string &source, const std::string &filename, const std::string& path, int level = 0);
	bool CompileShaders(const std::string& vs, const std::string& fs);

	virtual void DoAsyncWork(int mode, void *params) override;


	static bool ValidateProgram(unsigned int program);	
	static bool ValidateShader(GLuint shader, const char* file = 0);

	std::string m_VertexSrc;
	std::string m_FragmentSrc;

	std::string m_Name;

	unsigned int m_ShaderObject;
	unsigned int m_VSObject;
	unsigned int m_FSObject;
	
	std::map<std::string, int> m_ParamLocations;
};
