#include "Game.h"



//This function is called form a seperate thread.
//So we can do time intensive stuff in here without hanging the program
//HINT: DONT CALL FUNCTIONS THAT USE OPENGL IN HERE (i.e. Mesh::Create etc) DO THIS IN OnLoadContent!
void MyGame::Init()
{
	LOG_DEBUG("Initializing game");
	
	//binding escape key to exit function
	Input::BindKey(KEY_ESCAPE, MyGame::Exit, m_Framework);
}

void MyGame::OnLoadContent()
{
	
	CreateQuad();
	//and move it a bit backwards, so we can see it
	m_QuadMesh->GetTransform()->Translate(0, 0, -5);
}

void MyGame::OnUpdate(double deltaTime)
{
	//let it manually rotate around with quaternions, YAY!
	glm::mat4 currentMatrix = m_QuadMesh->GetTransform()->GetModelMatrix();
	glm::mat4 newMatrix = currentMatrix * Quaternion::Rotate(0.015, 0, 1, 0); //if we switch the order here (quat * currentMatrix) we would rotate around the world center
	m_QuadMesh->GetTransform()->SetModelMatrix(newMatrix);
}

void MyGame::OnRender()
{
	//First we enable the default unlit shader
	Shader::DefaultUnlit->Use();

	//Now set a color for the mesh
	Shader::DefaultUnlit->SetDiffuseColor(1, 1, 1);

	//and render it using the default unlit shader
	m_QuadMesh->Render(Shader::DefaultUnlit);
}

void MyGame::OnShutdown()
{
	//delete the mesh we created
	delete m_QuadMesh;
}

void MyGame::Exit(void* data)
{
	//we want to exit the game if this function is called
	((NoWork*)data)->Exit();
}

void MyGame::CreateQuad()
{
	//Lets create a quad object.
	//First we need 4 vertices
	std::vector<Vertex> vertices = { Vertex(glm::vec3(-1, -1, 1)),
		Vertex(glm::vec3(1, -1, 1)),
		Vertex(glm::vec3(1, 1, 1)),
		Vertex(glm::vec3(-1, 1, 1)),
		Vertex(glm::vec3(-1, -1, -1)),
		Vertex(glm::vec3(1, -1, -1)),
		Vertex(glm::vec3(1, 1, -1)),
		Vertex(glm::vec3(-1, 1, -1)) };

	//now we set up the indices(faces/polys). These have to be triangles.
	//indices are the index of the vertex to use for the face
	//   3     2
	//   .     .
	//   |\
			//   |  \
			//   |   \
			//   |____}
	//   .     .
	//   0     1
	std::vector<Face> faces = { Face(0, 1, 3), Face(1, 2, 3), Face(1, 5, 2), Face(5, 6, 2), Face(5, 4, 6),
		Face(4, 7, 6), Face(4, 0, 7), Face(0, 3, 7), Face(3, 2, 7), Face(2, 6, 7), Face(1, 0, 4), Face(1, 4, 5) };

	//now create the mesh
	m_QuadMesh = Mesh::Create(m_Renderer, vertices, faces);
}