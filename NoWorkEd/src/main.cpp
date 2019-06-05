#include <NoWork/NoWork.h>
#include "Editor.h"

int main(int argc, char** argv)
{
	//we need the framework itself
	NoWork framework;

	//and a game class object
	Editor editor;

	//TODO: Load config file for UI layout, window size etc.

	//Lets try to get the latest opengl version
	framework.SetOpenGLVersion(4, 5);

	//now create a window (with debug context)
	if (!framework.CreateNewWindow("NoWorkEd", 1280, 720, 40, 40, Window::Flags::WINDOW_SHOWED | Window::Flags::WINDOW_RESIZABLE | Window::Flags::WINDOW_OPENGL_DEBUG_CONTEXT))
		return -1;

	//now give the framework our game class object
	framework.RegisterGame(&editor);

	//run the game
	framework.Run();

	//since the destructor is deleting everything for us we are finished here
	return 0;
}