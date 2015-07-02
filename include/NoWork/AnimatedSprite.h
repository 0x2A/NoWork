#pragma once

#include "nowork/Common.h"
#include "nowork/SpriteAnimation.h"
#include "nowork/SceneObject.h"

class AnimatedSprite : public SceneObject
{
public:

	NOWORK_API static AnimatedSprite *Create();

	NOWORK_API void AddAnimation(SpriteAnimation* animation);
	NOWORK_API void AddAnimation(const std::vector<SpriteAnimation*>& animation);
	
	NOWORK_API void SelectAnimation(unsigned int index);
	NOWORK_API unsigned int CurrentAnimation(){ return m_CurrentAnimation; }

	NOWORK_API void Play();
	NOWORK_API void Play(unsigned int index);
	NOWORK_API void Pause();
	NOWORK_API void Stop();

	NOWORK_API void Update(double deltaTime);
	NOWORK_API void Render();

	NOWORK_API SpriteAnimation* GetAnimation(unsigned int index);
	NOWORK_API SpriteAnimation* GetCurrentAnimation();

	NOWORK_API void Loop(bool b);
	NOWORK_API void Loop(unsigned int index, bool b);

protected:

	AnimatedSprite();

private:

	bool m_Paused, m_Stopped;

	int m_CurrentAnimation;
	std::vector<SpriteAnimation*> m_Animations;
};