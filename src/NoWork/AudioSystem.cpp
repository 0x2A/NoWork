
#ifdef NOWORK_ENABLE_AUDIO

#include "NoWork/AudioSystem.h"
#include "NoWork/Log.h"
#include <fmod_errors.h>

#pragma comment(lib, "fmod64_vc.lib")

FMOD::System* AudioSystem::m_AudioSystem = nullptr;

bool AudioSystem::Init()
{
	//initialize the audio subsystem
	FMOD_RESULT result = FMOD::System_Create(&m_AudioSystem);
	if (!FMODErrorCheck(result))
		return false;

	// Check version
	unsigned int version;
	result = m_AudioSystem->getVersion(&version);
	if (!FMODErrorCheck(result))
		return false;

	if (version < FMOD_VERSION)
	{
		LOG_ERROR("Error! You are using an old version of FMOD (" << version << "). This program requires " << FMOD_VERSION);
		return false;
	}
	// Get number of sound cards 
	int numDrivers;
	result = m_AudioSystem->getNumDrivers(&numDrivers);
	if (!FMODErrorCheck(result))
		return false;

	// No sound cards (disable sound)
	if (numDrivers == 0)
	{
		result = m_AudioSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		if (!FMODErrorCheck(result))
			return false;

		LOG_WARNING("Audio subsystem:\n No audio device found. Audio output disabled.");
	}
	else // At least one sound card
	{
		// Get the capabilities of the default (0) sound card
		char driverName[255];
		int systemRate;
		FMOD_SPEAKERMODE speakerMode;
		int speakerModeChannels;
		result = m_AudioSystem->getDriverInfo(0, driverName, 255, 0, &systemRate, &speakerMode, &speakerModeChannels);
		FMODErrorCheck(result);

		LOG_MESSAGE("Initializing audio subsystem:\n using audio device '" << driverName << "', bitrate: " << systemRate << " kHz, channels: " << speakerModeChannels);		
	}


	FMOD_INITFLAGS initFlags = FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_CHANNEL_LOWPASS;
#ifdef _DEBUG
	initFlags |= FMOD_INIT_PROFILE_ENABLE;
#endif
	result = m_AudioSystem->init(512, initFlags, 0);    // Initialize FMOD.
	if (!FMODErrorCheck(result))
		return false;


	return true;
}

void AudioSystem::Shutdown()
{
	if (m_AudioSystem)
		m_AudioSystem->release();
}

void AudioSystem::Update()
{
	m_AudioSystem->update();
}

bool AudioSystem::FMODErrorCheck(FMOD_RESULT result)
{	
	if (result != FMOD_OK)
	{
		LOG_ERROR("Audio subsystem: error (" << result << "): " << FMOD_ErrorString(result));
		return false;
	}
	return true;
}

#endif