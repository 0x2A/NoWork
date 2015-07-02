#pragma once

#include "NoWork/Common.h"
#include <fmod.hpp>

class AudioSystem
{
	friend class AudioSource;
public:
	static bool Init();
	static void Shutdown();
	static void Update();

protected:

	static FMOD::System* m_AudioSystem;

private:
	
	static bool FMODErrorCheck(FMOD_RESULT result);

};