#pragma once

#include <fmod.hpp>
#include "NoWork/Common.h"
#include "NoWork/SceneObject.h"


class AudioSource : public SceneObject
{
	friend class NoWork;
public:

	NOWORK_API static AudioSource* Load(const std::string& name, const std::string& filename, bool ambient = false, bool stream = true);
	NOWORK_API ~AudioSource();

	NOWORK_API void SetGain(float f);
	NOWORK_API float GetGain();

	NOWORK_API void Play();
	NOWORK_API void Pause();
	NOWORK_API void Stop();

	NOWORK_API bool IsPlaying();
	NOWORK_API bool IsPaused();
	NOWORK_API bool IsStopped();

	NOWORK_API void SetLooping(bool state = true);
	NOWORK_API bool IsLooping();

	NOWORK_API void SetAmbient(bool state = true);

	//Sets the reverb for this channel. You can use FMOD_PRESET_* presets for the properties.
	//HINT: THIS ONLY WORKS AFTER Play() IS CALLED!
	NOWORK_API void SetReverb(FMOD_REVERB_PROPERTIES props = FMOD_PRESET_BATHROOM, float wetMix = 0.5f, float DryMix = 0.5f);

	//Removes the reverb effect
	NOWORK_API void RemoveReverb();

	AudioSource();
	
private:
	bool m_Streaming;
	FMOD::Sound* m_Source;
	FMOD::Channel* m_Channel;
	std::string m_Filename;
	FMOD::DSP* m_Reverb;
};