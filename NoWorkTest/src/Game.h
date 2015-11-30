#include <NoWork/NoWork.h>


class MyGame : public GameBase
{
public:
	//This function is called form a seperate thread.
	//So we can do time intensive stuff in here without hanging the program
	//HINT: DONT CALL FUNCTIONS THAT USE OPENGL IN HERE (i.e. Mesh::Create etc) DO THIS IN OnLoadContent!
	virtual void Init();
	
	//Here we can load resources, meshes etc. (its thread save)
	virtual void OnLoadContent();

	//This is called every frame to update our logic and move things around
	virtual void OnUpdate(double deltaTime);

	//This is called every time the frame renders. Here we can call render functions etc.
	virtual void OnRender();

	//This is called if the program shuts down. Here we should delete all the content we created ourself
	virtual void OnShutdown();


	//Bound to escape key, used to exit the game when escape pressed
	void Exit();

	virtual void OnLoadRender() override;

private:


	SpriteSheet *m_SpriteSheet;

	AnimatedSprite *m_AnimatedSprite;
	AudioSource *m_Sound;

	Model* m_Model;

	Framebuffer* m_FBO;
	RenderTexture* m_RenderTex;
};