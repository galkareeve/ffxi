#include "StdAfx.h"
#include "myStruct.h"
#include "MyMathLib.h"

#include <math.h>

CMyMathLib::CMyMathLib(void)
{
}

CMyMathLib::~CMyMathLib(void)
{
}

float CMyMathLib::dotProd(myVector *first, myVector *second)
{
	return ((first->x*second->x) + (first->y*second->y) + (first->z*second->z));
}

float CMyMathLib::mag(myVector *in)
{
	return sqrt( in->x*in->x + in->y*in->y + in->z*in->z );
}

float CMyMathLib::angleBtwVector(myVector *first, myVector *second)
{
	return (acos(dotProd(first,second) / (mag(first) * mag(second)))  * 180.0 / PI);
}

myVector CMyMathLib::bezierCubic(float u, myVector p0, myVector p1, myVector p2, myVector p3)
{
	myVector result;

	float u1 = 1.0f-u;
	result.x = bezierInternal(u,u1,p0.x,p1.x,p2.x,p3.x);
	result.y = bezierInternal(u,u1,p0.y,p1.y,p2.y,p3.y);
	result.z = bezierInternal(u,u1,p0.z,p1.z,p2.z,p3.z);

	return result;
}

float CMyMathLib::bezierInternal(float u, float u1, float x0, float x1, float x2, float x3)
{
	return ( u1*u1*u1*x0 + 3.0f*u1*u1*u*x1 + 3.0f*u1*u*u*x2 + u*u*u*x3 );
}

myVector CMyMathLib::straightLine(float t, myVector p0, myVector p1)
{
	myVector result;

	result.x = (1.0f-t)*p0.x + t*p1.x;
	result.y = (1.0f-t)*p0.y + t*p1.y;
	result.z = (1.0f-t)*p0.z + t*p1.z;

	return result;
}


//quaternion, normalize axis vector and quaternion
//W=cos(t/2)
//X=x*sin(t/2)
//Y=y*sin(t/2)
//Z=z*sin(t/2)

//q' = q*V*q-,  where q- = q* if q is normalized, and V = vertices to rotate
//q- = q*/q.q*, when q is normalized, q.q*=1, therefore q- = q*
