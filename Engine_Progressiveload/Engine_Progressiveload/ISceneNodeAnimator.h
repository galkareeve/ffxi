#pragma once

#include <glm/glm.hpp>

#include "myEnum.h"
#include "IReferenceCounted.h"

//! Animates a scene node. Can animate position, rotation, material, and so on.
/** A scene node animator is able to animate a scene node in a very simple way. It may
change its position, rotation, scale and/or material. There are lots of animators
to choose from. You can create scene node animators with the ISceneManager interface.
*/
class ISceneNode;
class ISceneNodeAnimator : public IReferenceCounted
{
public:
	//! Animates a scene node.
	/** \param node Node to animate.
	\param timeMs Current time in milli seconds. */
	virtual void animateNode(ISceneNode* node, unsigned int timeMs) =0;

	//! Returns type of the scene node animator
	virtual ESCENE_NODE_ANIMATOR_TYPE getType() const
	{
		return ESNAT_UNKNOWN;
	}

	//! Returns if the animator has finished.
	/** This is only valid for non-looping animators with a discrete end state.
	\return true if the animator has finished, false if it is still running. */
	virtual bool hasFinished(void) const
	{
		return false;
	}
};
