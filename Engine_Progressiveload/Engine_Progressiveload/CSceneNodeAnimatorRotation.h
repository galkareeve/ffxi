#pragma once
#include "ISceneNode.h"
#include "ISceneNodeAnimator.h"

class CSceneNodeAnimatorRotation : public ISceneNodeAnimator
{
public:

	//! constructor
	CSceneNodeAnimatorRotation(unsigned int time, const glm::fvec3 &rotation);

	//! animates a scene node
	virtual void animateNode(ISceneNode* node, unsigned int timeMs);

	//! Returns type of the scene node animator
	virtual ESCENE_NODE_ANIMATOR_TYPE getType() const  { return ESNAT_ROTATION; }

private:
	glm::fvec3 Rotation;
	unsigned int StartTime;
};
