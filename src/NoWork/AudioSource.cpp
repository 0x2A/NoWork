#include "NoWork/AudioSource.h"
#include "NoWork/Log.h"
#include "NoWork/AudioSystem.h"


NOWORK_API  AudioSource* AudioSource::Load(const std::string& name, const std::string& filename, bool ambient /*= false*/, bool stream)
{
	AudioSource* audioSource = new AudioSource;
	if (!AudioSystem::m_AudioSystem)
	{
		LOG_ERROR("Unable to load audio file '" << filename << "': audio subsystem not initialized!");
		return audioSource;
	}
	LOG_DEBUG("loading audio file '" << filename << "'");
	
	if (filename.empty())
		return audioSource;

	
	//Create a IAudio object and load a sound from a file
	audioSource->m_Streaming = stream;

	FMOD_MODE modes = ambient ? FMOD_3D_HEADRELATIVE : FMOD_3D;
	FMOD_RESULT result;
	if (stream)
		result = AudioSystem::m_AudioSystem->createStream(filename.c_str(), modes, 0, &audioSource->m_Source);
	else
		result = AudioSystem::m_AudioSystem->createSound(filename.c_str(), modes, 0, &audioSource->m_Source);
		
	if (result != FMOD_OK)
	{
		LOG_ERROR("Unable to load audio file '" << filename << "'");
	}
	audioSource->m_Filename = filename;

	return audioSource;
}

AudioSource::AudioSource()
{
	m_Source = nullptr;
	m_Channel = nullptr;
	m_Reverb = nullptr;
}


AudioSource::~AudioSource()
{
	if (m_Source)
		m_Source->release();
}

void AudioSource::SetGain(float f)
{
	if (m_Channel)
		m_Channel->setVolume(f);
}

float AudioSource::GetGain()
{
	float vol = 0;
	if (m_Channel) m_Channel->getVolume(&vol);
	return vol;
}

void AudioSource::Play()
{
	if (m_Source)
	{
		AudioSystem::m_AudioSystem->playSound(m_Source, 0, false, &m_Channel);
		
	}
}

void AudioSource::Pause()
{
	if (!m_Channel) return;
	m_Channel->setPaused(true);
}

void AudioSource::Stop()
{
	if (m_Channel)
		m_Channel->stop();
}

bool AudioSource::IsPlaying()
{
	if (!m_Channel) return false;
	bool state;
	m_Channel->isPlaying(&state);
	return state;
}

bool AudioSource::IsPaused()
{
	if (!m_Channel) return false;
	bool state;
	m_Channel->isPlaying(&state);
	return !state;
}

bool AudioSource::IsStopped()
{
	if (!m_Channel) return true;
	bool state;
	m_Channel->isPlaying(&state);
	return !state;
}

void AudioSource::SetAmbient(bool state /*= true*/)
{
	if (state)
		m_Source->setMode(FMOD_3D_HEADRELATIVE);
	else
		m_Source->setMode(FMOD_3D);
}


void AudioSource::SetLooping(bool state /*= true*/)
{
	if (!m_Source) return;
	if (state)
	{
		m_Source->setMode(FMOD_LOOP_NORMAL);
		m_Source->setLoopCount(-1);

	}
	else
		m_Source->setLoopCount(0);
	if (m_Channel)
	{
		if (state)
			m_Source->setLoopCount(-1);
		else
			m_Source->setLoopCount(0);
	}
}

bool AudioSource::IsLooping()
{
	int loop;
	m_Source->getLoopCount(&loop);
	return loop != 0;
}

NOWORK_API void AudioSource::SetReverb(FMOD_REVERB_PROPERTIES props, float wetMix, float DryMix)
{
	if (!m_Channel) return;

	if (!m_Reverb)
	{
		AudioSystem::m_AudioSystem->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &m_Reverb);
	}
	
	float *param = (float *)&props;
	int numParams;
	m_Reverb->getNumParameters(&numParams);
	for (int i = 0; i < numParams; i++, param++)
	{
		m_Reverb->setParameterFloat(i, *param);
	}
	m_Reverb->setWetDryMix(1.0f, wetMix, DryMix);
	m_Channel->addDSP(0, m_Reverb);
}

NOWORK_API void AudioSource::RemoveReverb()
{
	if (!m_Channel) return;
	if (!m_Reverb) return;
	m_Channel->removeDSP(m_Reverb);

}

