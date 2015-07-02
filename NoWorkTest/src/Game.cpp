#include "Game.h"



//This function is called form a seperate thread.
//So we can do time intensive stuff in here without hanging the program
//HINT: DONT CALL FUNCTIONS THAT USE OPENGL IN HERE (i.e. Mesh::Create etc) DO THIS IN OnLoadContent!
void MyGame::Init()
{
	LOG_DEBUG("Initializing game");
	
	m_Sound = AudioSource::Load("sound", "data/sound.ogg", true);
	m_Sound->SetGain(0.7f);
	m_Sound->SetLooping(true);
	m_Sound->Play();
	m_Sound->SetReverb(FMOD_PRESET_CONCERTHALL, 0.75f, 0.25f);
	
	//binding escape key to exit function
	Input::BindKey(KEY_ESCAPE, this, &MyGame::Exit);
}

void MyGame::OnLoadContent()
{
	//create a quad mesh
	CreateQuad();
	//and move it a bit backwards, so we can see it
	m_QuadMesh->GetTransform()->Translate(0, 0, -5);

	//set the camera to orthographic projection, since we are using sprites
	m_Renderer->GetCamera()->SetOrthographic(true);
	//move the camera back a bit so we see something
	m_Renderer->GetCamera()->GetTransform()->Translate(glm::vec3(0, 0, -1));

	//load a spritesheet from json file
	m_SpriteSheet = SpriteSheet::Load("data/miku.json");
	//disable linear filtering (pixelated look)
	m_SpriteSheet->SetLinearFiltering(false);
	//remove the backround using color keying
	m_SpriteSheet->SetColorKey(glm::vec3(0.588f,0.784f,0.98f));

	//create an animated sprite
	m_AnimatedSprite = AnimatedSprite::Create();
	m_AnimatedSprite->AddAnimation(m_SpriteSheet->GetAnimations()); //add all animations from the spritesheet to the animated sprite
	m_AnimatedSprite->Play(0); //play the first animation

	//scale the sprite down a bit
	m_AnimatedSprite->GetTransform()->Scale(0.5f, 0.5f, 0.5f);

}

void MyGame::OnUpdate(double deltaTime)
{

	if (Input::KeyDown(KEY_1))
		m_AnimatedSprite->SelectAnimation(0);
	if (Input::KeyDown(KEY_2))
		m_AnimatedSprite->SelectAnimation(1);
	if (Input::KeyDown(KEY_3))
		m_AnimatedSprite->SelectAnimation(2);

	//update the animated sprite
	m_AnimatedSprite->Update(deltaTime);
}

void MyGame::OnRender()
{
	//First we enable the default unlit shader
	Shader::DefaultUnlit->Use();

	//Now set a color for the mesh
	Shader::DefaultUnlit->SetDiffuseColor(1, 1, 1);

	//and render it using the default unlit shader
	//m_QuadMesh->Render(Shader::DefaultUnlit);

	//render a sprite
	m_AnimatedSprite->Render();
}

void MyGame::OnShutdown()
{
	//delete the mesh we created
	DelPtr(m_SpriteSheet);
}

void MyGame::Exit()
{
	//we want to exit the game if this function is called
	m_Framework->Exit();
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
	m_QuadMesh = Mesh::Create(vertices, faces);
}