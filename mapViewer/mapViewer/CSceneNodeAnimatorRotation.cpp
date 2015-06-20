#include "CSceneNodeAnimatorRotation.h"

//! constructor
CSceneNodeAnimatorRotation::CSceneNodeAnimatorRotation(unsigned int time, const glm::fvec3 &rotation)
: Rotation(rotation), StartTime(time)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorRotation");
	#endif
}


//! animates a scene node
void CSceneNodeAnimatorRotation::animateNode(ISceneNode* node, unsigned int timeMs)
{
	if (node) // thanks to warui for this fix
	{
		const unsigned int diffTime = timeMs - StartTime;

		if (diffTime != 0)
		{
			// clip the rotation to small values, to avoid
			// precision problems with huge floats.
			glm::fvec3 rot = node->getRotation() + Rotation*(diffTime*0.1f);
			if (rot.x>360.f)
				rot.x=fmodf(rot.x, 360.f);
			if (rot.y>360.f)
				rot.y=fmodf(rot.y, 360.f);
			if (rot.z>360.f)
				rot.z=fmodf(rot.z, 360.f);
			node->setRotation(rot);
			StartTime=timeMs;
		}
	}
}
