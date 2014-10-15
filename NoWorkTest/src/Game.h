#include "../../src/NoWork.h"


class MyGame : public GameBase
{


	virtual void Init();

	virtual void OnUpdate(double deltaTime);

	virtual void OnRender();

	virtual void OnShutdown();

};