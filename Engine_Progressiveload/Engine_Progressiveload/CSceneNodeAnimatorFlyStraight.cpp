#include "CSceneNodeAnimatorFlyStraight.h"
#include "ISceneNode.h"

//! constructor
CSceneNodeAnimatorFlyStraight::CSceneNodeAnimatorFlyStraight(const glm::fvec3 &startPoint,
				const glm::fvec3 &endPoint, unsigned int timeForWay,
				bool loop, unsigned int now, bool pingpong)
: ISceneNodeAnimatorFinishing(now + timeForWay),
	Start(startPoint), End(endPoint), TimeFactor(0.0f), StartTime(now),
	TimeForWay(timeForWay), Loop(loop), PingPong(pingpong)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorFlyStraight");
	#endif

	recalculateIntermediateValues();
}


void CSceneNodeAnimatorFlyStraight::recalculateIntermediateValues()
{
	Vector = End - Start;
	TimeFactor = (float)glm::length(Vector) / TimeForWay;
	glm::normalize(Vector);
}


//! animates a scene node
void CSceneNodeAnimatorFlyStraight::animateNode(ISceneNode* node, unsigned int timeMs)
{
	if (!node)
		return;

	unsigned int t = (timeMs-StartTime);

	glm::fvec3 pos;

	if (!Loop && !PingPong && t >= TimeForWay) {
		pos = End;
		HasFinished = true;
	}
	else if (!Loop && PingPong && t >= TimeForWay * 2.f ) {
		pos = Start;
		HasFinished = true;
	}
	else {
		float phase = fmodf( (float) t, (float) TimeForWay );
		glm::fvec3 rel = Vector * phase * TimeFactor;
		const bool pong = PingPong && fmodf( (float) t, (float) TimeForWay*2.f ) >= TimeForWay;

		if ( !pong ) {
			pos += Start + rel;
		}
		else {
			pos = End - rel;
		}
	}
	node->setPosition(pos);
}
