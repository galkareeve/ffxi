#include "CSceneNodeAnimatorFlyCircle.h"


//! constructor
CSceneNodeAnimatorFlyCircle::CSceneNodeAnimatorFlyCircle(unsigned int time,
		const glm::fvec3 &center, float radius, float speed,
		const glm::fvec3 &direction, float radiusEllipsoid)
	: Center(center), Direction(direction), Radius(radius),
	RadiusEllipsoid(radiusEllipsoid), Speed(speed), StartTime(time)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorFlyCircle");
	#endif
	init();
}


void CSceneNodeAnimatorFlyCircle::init()
{
	glm::normalize(Direction);

	if (Direction.y != 0)
		VecV = glm::normalize(glm::cross(glm::fvec3(50,0,0), Direction));
	else
		VecV = glm::normalize(glm::cross(glm::fvec3(0,50,0), Direction));
		
	VecU = glm::normalize(glm::cross(VecV,Direction));
}


//! animates a scene node
void CSceneNodeAnimatorFlyCircle::animateNode(ISceneNode* node, unsigned int timeMs)
{
	if ( 0 == node )
		return;

	float time;

	// Check for the condition where the StartTime is in the future.
	if(StartTime > timeMs)
		time = ((int)timeMs - (int)StartTime) * Speed;
	else
		time = (timeMs-StartTime) * Speed;

	float r2 = RadiusEllipsoid == 0.f ? Radius : RadiusEllipsoid;
	node->setPosition(Center + (Radius*cosf(time)*VecU) + (r2*sinf(time)*VecV ) );
}

