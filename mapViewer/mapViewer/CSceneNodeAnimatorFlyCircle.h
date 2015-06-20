#pragma once

#include "ISceneNode.h"
#include "ISceneNodeAnimator.h"

class CSceneNodeAnimatorFlyCircle : public ISceneNodeAnimator
{
public:

	//! constructor
	CSceneNodeAnimatorFlyCircle(unsigned int time,	const glm::fvec3 &center, float radius,
				float speed, const glm::fvec3 &direction,
				float radiusEllipsoid);

	//! animates a scene node
	virtual void animateNode(ISceneNode* node, unsigned int timeMs);

	//! Returns type of the scene node animator
	virtual ESCENE_NODE_ANIMATOR_TYPE getType() const { return ESNAT_FLY_CIRCLE; }

private:
	// do some initial calculations
	void init();

	// circle center
	glm::fvec3 Center;
	// up-vector, normal to the circle's plane
	glm::fvec3 Direction;
	// Two helper vectors
	glm::fvec3 VecU;
	glm::fvec3 VecV;
	float Radius;
	float RadiusEllipsoid;
	float Speed;
	unsigned int StartTime;
};

