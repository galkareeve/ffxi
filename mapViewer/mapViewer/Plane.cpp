// Plane.cpp
//
//////////////////////////////////////////////////////////////////////

#include "Plane.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>

Plane::Plane( glm::vec3 &v1,  glm::vec3 &v2,  glm::vec3 &v3)
{
	set3Points(v1,v2,v3);
}

Plane::Plane() {}

Plane::~Plane() {}


void Plane::set3Points( glm::vec3 &v1,  glm::vec3 &v2,  glm::vec3 &v3)
{
	glm::vec3 aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	normal = glm::cross(aux2, aux1);
	normal = glm::normalize(normal);

	point = v2;
	d = -(glm::dot(normal,point));
}

void Plane::setNormalAndPoint(glm::vec3 &normal, glm::vec3 &point) 
{
	this->normal = normal;
	this->normal = glm::normalize(this->normal);
	d = -(glm::dot(normal,point));
}

void Plane::setCoefficients(float a, float b, float c, float d) 
{
	// set the normal vector
	normal.x=a;
	normal.y=b;
	normal.z=c;
	//compute the lenght of the vector
//	float l = normal.length();
	float l = glm::length(normal);
	// normalize the vector
	normal.x=a/l;
	normal.y=b/l;
	normal.z=c/l;
	// and divide d by th length as well
	this->d = d/l;
}

float Plane::distance(glm::vec3 &p) 
{
	return (d + glm::dot(normal,p));
}

void Plane::set2BoxAxis(glm::mat4 &mInv)
{
//	normalT = glm::vec3(mInv * glm::vec4(normal,0));
//	normalT = glm::normalize(normalT);

	//transform plane normal to box axis
	glm::vec3 bx = glm::vec3(glm::column(mInv,0));
//	bx = glm::normalize(bx);
	glm::vec3 by = glm::vec3(glm::column(mInv,1));
//	by = glm::normalize(by);
	glm::vec3 bz = glm::vec3(glm::column(mInv,2));
//	bz = glm::normalize(bz);

	normalT.x = glm::dot(bx, normal);
	normalT.y = glm::dot(by, normal);
	normalT.z = glm::dot(bz, normal);
//	normalT = glm::normalize(normalT);
}

float Plane::distanceT(glm::vec3 &p)
{
	return (d + glm::dot(normalT,p));
}
