#include "Game.h"

void MyGame::Init()
{
	//throw std::exception("The method or operation is not implemented.");
	LOG_DEBUG("Initializing game");
	
	//binding escape key to exit function
	Input::BindKey(KEY_ESCAPE, MyGame::Exit, m_Framework);

}

void MyGame::OnUpdate(double deltaTime)
{

}

void MyGame::OnRender()
{
}

void MyGame::OnShutdown()
{
}

void MyGame::Exit(void* data)
{
	((NoWork*)data)->Exit();
}