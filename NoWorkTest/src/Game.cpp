#include "Game.h"



const float moveSpeed = 45.f;
bool useWireframe;

// This function is for preinitializing resources needed 
// while loading content or while loading screen
// Its running on the main thread so don't run too time 
// intensive stuff here as it may cause a window freeze
void MyGame::Init()
{
	LOG_DEBUG("Initializing game");

	
	//binding escape key to exit function
	Input::BindKey(KEY_ESCAPE, this, &MyGame::Exit);

	useWireframe = false;
}

// This function is running in a seperate thread so
// you can run time intensive stuff here, while in the
// main window a loading screen or similar (depending on
// code in OnLoadRender function) is rendered. OpenGL 
// calls are done via command stack and are therefore not
// guaranteed to be processed in order they are called!
void MyGame::OnLoadContent()
{
	//Set anisotropic filtering (if available)
	m_Renderer->SetAnisotropicFiltering(16.0f);

	//move the camera back a bit so we see something
	m_Renderer->GetCamera()->GetTransform()->Translate(glm::vec3(0, 0, -10));

	//load a spritesheet from json file
	m_SpriteSheet = SpriteSheet::Load("data/miku.json");
	
	if (m_SpriteSheet)
	{
		//disable linear filtering (pixelated look)
		m_SpriteSheet->SetLinearFiltering(false);
		//remove the backround using color keying
		m_SpriteSheet->SetColorKey(glm::vec3(0.588f, 0.784f, 0.98f));
	
		//create an animated sprite
		m_AnimatedSprite = AnimatedSprite::Create();
		m_AnimatedSprite->AddAnimation(m_SpriteSheet->GetAnimations()); //add all animations from the spritesheet to the animated sprite
		m_AnimatedSprite->Play(0); //play the first animation

		//scale the sprite down a bit
		m_AnimatedSprite->GetTransform()->Scale(0.5f, 0.5f, 0.5f);
	}
	//load a model from file
	m_Model = Model::Load("data/altair/altair.3ds");

	//create a framebuffer object for testing
	m_FBO = Framebuffer::Create(1280, 720);

}

//update function
void MyGame::OnUpdate(double deltaTime)
{
	//process input so we can move the camera around with wsad, q and r
	auto camera = m_Renderer->GetCamera();

	if (m_AnimatedSprite)
	{
		if (Input::KeyDown(KEY_1))
			m_AnimatedSprite->SelectAnimation(0);
		if (Input::KeyDown(KEY_2))
			m_AnimatedSprite->SelectAnimation(1);
		if (Input::KeyDown(KEY_3))
			m_AnimatedSprite->SelectAnimation(2);
	}
	if (Input::KeyHeld(KEY_W))
		camera->GetTransform()->Translate(glm::vec3(0, 0, moveSpeed*deltaTime));
	if (Input::KeyHeld(KEY_S))
		camera->GetTransform()->Translate(glm::vec3(0, 0, -moveSpeed*deltaTime));
	if (Input::KeyHeld(KEY_A))
		camera->GetTransform()->Translate(glm::vec3(moveSpeed*deltaTime, 0, 0));
	if (Input::KeyHeld(KEY_D))
		camera->GetTransform()->Translate(glm::vec3(-moveSpeed*deltaTime, 0, 0));
	if (Input::KeyHeld(KEY_E))
		camera->GetTransform()->Translate(glm::vec3(0, moveSpeed*deltaTime, 0));
	if (Input::KeyHeld(KEY_Q))
		camera->GetTransform()->Translate(glm::vec3(0, -moveSpeed*deltaTime, 0));
	if (Input::KeyDown(KEY_O))
	{
		useWireframe = !useWireframe;
		m_Renderer->SetWireframeMode(useWireframe);
	}

	//update the animated sprite
	if(m_AnimatedSprite) m_AnimatedSprite->Update(deltaTime);
}

void MyGame::OnLoadRender()
{
	//TODO: print a loading message or render a loading screen
}

//main render function
void MyGame::OnRender()
{
	//render a sprite
	if (m_AnimatedSprite)
		m_AnimatedSprite->Render();

	//render the model
	//m_Model->Render(Shader::DefaultUnlitTextured);
}

//used to clean up resources
void MyGame::OnShutdown()
{
	//delete the mesh we created
	m_FBO->Unbind();
}

//exit function used to demonstrate the input key binding functionality
void MyGame::Exit()
{
	//we want to exit the game if this function is called
	m_Framework->Exit();
}
