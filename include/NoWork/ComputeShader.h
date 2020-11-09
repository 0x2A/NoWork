/*
*  ===========================================================================
*	This file is part of CrystalEngine source code.
*   Copyright (C) 2020  Frank Köhnke
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
#include "NoWork/Shader.h"

class ComputeShader;
typedef std::shared_ptr<ComputeShader> ComputeShaderPtr;

class ComputeShader : public Shader
{
	friend class NoWork;

public:

	NOWORK_API virtual ~ComputeShader();
	NOWORK_API static ComputeShaderPtr Create(const char* name, const char* cs);
	NOWORK_API static ComputeShaderPtr Load(const char* computeShaderPath);


	NOWORK_API void Dispatch(uint num_groups_x, uint num_groups_y, uint num_groups_z);

	
	NOWORK_API void BindImageTexture(uint slot, TexturePtr targetTexture, int level = 0);

protected:

	bool CompileShaders(const std::string& cs);

	NOWORK_API ComputeShader();

	virtual void DoAsyncWork(int mode, void *params) override;

private:

	std::string m_ShaderSrc;
	unsigned int m_CSObject;
};