#include "Game.h"

Mesh *testmesh;

void MyGame::Init()
{
	//throw std::exception("The method or operation is not implemented.");
	LOG_DEBUG("Initializing game");
	
	//binding escape key to exit function
	Input::BindKey(KEY_ESCAPE, MyGame::Exit, m_Framework);


	std::vector<Vertex> vertices = { Vertex(glm::vec3(0, 0, 0)),
									Vertex(glm::vec3(1, 0, 0)) };
	testmesh = Mesh::Create(vertices);

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