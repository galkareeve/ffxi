#pragma once

#include "ISceneNodeAnimator.h"

//! This is an abstract base class for animators that have a discrete end time.
class ISceneNodeAnimatorFinishing : public ISceneNodeAnimator
{
public:

	//! constructor
	ISceneNodeAnimatorFinishing(unsigned int finishTime)
		: FinishTime(finishTime), HasFinished(false) { }

	virtual bool hasFinished(void) const { return HasFinished; }

protected:
	unsigned int FinishTime;
	bool HasFinished;
};

