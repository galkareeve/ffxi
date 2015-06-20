#pragma once

#include "ISceneNodeAnimatorFinishing.h"

class CSceneNodeAnimatorFlyStraight : public ISceneNodeAnimatorFinishing
{
public:

	//! constructor
	CSceneNodeAnimatorFlyStraight(const glm::fvec3 &startPoint,
						const glm::fvec3 &endPoint,	unsigned int timeForWay,
						bool loop, unsigned int now, bool pingpong);

	//! animates a scene node
	virtual void animateNode(ISceneNode* node, unsigned int timeMs);

	//! Returns type of the scene node animator
	virtual ESCENE_NODE_ANIMATOR_TYPE getType() const { return ESNAT_FLY_STRAIGHT; }

	private:

	void recalculateIntermediateValues();

	glm::fvec3 Start;
	glm::fvec3 End;
	glm::fvec3 Vector;
	float TimeFactor;
	unsigned int StartTime;
	unsigned int TimeForWay;
	bool Loop;
	bool PingPong;
};


