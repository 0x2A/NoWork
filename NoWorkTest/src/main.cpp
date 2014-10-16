#include "../../src/NoWork.h"
#include "Game.h"


int main(int argc, char** argv)
{
	NoWork framework;
	MyGame game;

	framework.SetOpenGLVersion(4, 5);
	if (!framework.CreateNewWindow("Test", 1280, 720, 40, 40, Window::Flags::WINDOW_WINDOWED | Window::Flags::WINDOW_OPENGL_DEBUG_CONTEXT))
		return -1;

	
	framework.RegisterGame(&game);

	framework.Run();


	return 0;
}