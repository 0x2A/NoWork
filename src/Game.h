#pragma once

#include "Framework.h"
#include "Renderer.h"

//Base game interface
class GameBase
{
	friend class NoWork;
public:
	virtual void Init() = 0; //called just before main loop (for loading stuff)
	virtual void OnUpdate(double deltaTime) = 0; //called if game should update
	virtual void OnRender() = 0; //called while rendering process
	virtual void OnShutdown() = 0; //called when framework is shutting down

protected:
	NoWork* m_Framework;
	Renderer* m_Renderer;
};