#include "../../src/NoWork.h"


class MyGame : public GameBase
{
public:

	virtual void Init();

	virtual void OnUpdate(double deltaTime);

	virtual void OnRender();

	virtual void OnShutdown();


	//Bound to escape key, used to exit the game when escape pressed
	void static Exit(void* data);
private:
	quaternion quat;
};