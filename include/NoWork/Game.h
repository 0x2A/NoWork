#pragma once

#include "nowork/Framework.h"
#include "nowork/Renderer.h"

//Base game interface
class GameBase
{
	friend class NoWork;
public:
	virtual void Init() = 0; //called to initialize huge stuff (like loading some files etc. Hint: Dont create renderable stuff like Meshes in here! Do it in OnLoadContent!!!
	virtual void OnLoadContent() = 0; //Load rendereable stuff and things using opengl functions here. Dont do this in Init, since its called from another thread!
	virtual void OnUpdate(double deltaTime) = 0; //called if game should update
	virtual void OnRender() = 0; //called while rendering process
	virtual void OnShutdown() = 0; //called when framework is shutting down

protected:
	NoWork* m_Framework;
	Renderer* m_Renderer;
};