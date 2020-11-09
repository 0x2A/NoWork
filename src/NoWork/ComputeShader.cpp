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


#include "nowork/ComputeShader.h"

#include "nowork/Mesh.h"
#include "nowork/Framework.h"
#include "NoWork/FileSystem.h"



NOWORK_API ComputeShader::~ComputeShader()
{
	glDetachShader(m_ShaderObject, m_CSObject);

	glDeleteShader(m_CSObject);

}

NOWORK_API ComputeShaderPtr ComputeShader::Create(const char* name, const char* cs)
{
	ComputeShaderPtr shader = ComputeShaderPtr(new ComputeShader);
	shader->m_Name = name;

	if (!NoWork::IsMainThread())
	{
		std::string* arr = new std::string[1];
		arr[0] = cs;
		shader->AddToGLQueue(0, arr);
		return shader;
	}

	if (!shader->CompileShaders(cs))
	{
		return nullptr;
	}

	return shader;
}

NOWORK_API ComputeShaderPtr ComputeShader::Load(const char* computeShaderPath)
{
	LOG_DEBUG("Creating shader from file '" << computeShaderPath << "'");

	std::string computeSrc = FileSystem::LoadTextFile(computeShaderPath);
	computeSrc = PreprocessIncludes(computeSrc, FileSystem::GetFilename(computeShaderPath), FileSystem::GetPath(computeShaderPath));
	// Remove directory if present.
	// Do this before extension removal incase directory has a period character.

	std::string filename = computeShaderPath;

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

	return Create(filename.c_str(), computeSrc.c_str());
}


NOWORK_API void ComputeShader::Dispatch(uint num_groups_x, uint num_groups_y, uint num_groups_z)
{
	glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
}

NOWORK_API void ComputeShader::BindImageTexture(uint slot, TexturePtr targetTexture, int level /*= 0*/)
{
	glBindImageTexture(slot, targetTexture->GetTextureId(), level, GL_TRUE, 0, GL_WRITE_ONLY, targetTexture->GetFormat());
}

bool ComputeShader::CompileShaders(const std::string& cs)
{
	m_CSObject = glCreateShader(GL_COMPUTE_SHADER);

	GLchar const* filesCS[]{cs.c_str()};

	glShaderSource(m_CSObject, 1, filesCS, 0);
	glCompileShader(m_CSObject);
	if (!ValidateShader(m_CSObject))
	{
		return false;
	}

	m_ShaderObject = glCreateProgram();
	glAttachShader(m_ShaderObject, m_CSObject);

	glLinkProgram(m_ShaderObject);
	if (!ValidateProgram(m_ShaderObject))
	{
		return false;
	}

	return true;
}

NOWORK_API ComputeShader::ComputeShader()
{}

void ComputeShader::DoAsyncWork(int mode, void *params)
{
	std::string* arr = static_cast<std::string*>(params);
	CompileShaders(arr[0]);
}
