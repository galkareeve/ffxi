#include "Frustum.h"
// Include GLFW
#include <glfw/glfw3.h>
//extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#define ANG2RAD 3.14159265358979323846/180.0
CFrustum::CFrustum(void)
{
	ratio=nearD=farD=nw=nh=fw=fh=0;
	
	// Initial position : on +Z
	positionT = position = glm::vec3( 0, -15, 0 ); 
	// Initial horizontal angle : toward -Z
//	hAT = horizontalAngle = 3.14f;
	hAT = horizontalAngle = ANG2RAD;
	// Initial vertical angle : none
	vAT = verticalAngle = 0.0f;
	// Initial Field of View (45.0f)
	initialFoV = 45.0f;

	speed = 15.0f; // 3 units / second
	mouseSpeed = 0.005f;
	isFocus=true;
	isDualCamera=false;
	isEye=false;
}


CFrustum::~CFrustum(void)
{
}

void CFrustum::setFrustumPos(float extend)
{
	float midpoint = extend/2;
	positionT = position = glm::vec3( midpoint, -15, midpoint );
}

//set the frustum culling parameter which in turn defines the clipping plane, this is different from the projection matrix
void CFrustum::setCamInternals(float angle, float ratio, float nearD, float farD) 
{
	// store the information
	this->ratio = ratio;
	this->angle = angle;
	this->nearD = nearD;
	this->farD = farD;

	// compute width and height of the near section
	tang = (float)tan(ANG2RAD * angle * 0.5) ;
	nh = nearD * tang;
	nw = nh * ratio; 
	fh = farD  * tang;
	fw = fh * ratio;
}

//call by computeMatricesFromInputs
void CFrustum::setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u) 
{
	cc = p;
	glm::vec3 dir,nc,fc,X,Y,Z;

	Z = p - l;
	Z = glm::normalize(Z);

	X = glm::cross(u, Z);
	X = glm::normalize(X);

	Y = glm::cross(Z, X);

	nc = p - Z * nearD;
	fc = p - Z * farD;

	ntl = nc + Y * nh - X * nw;
	ntr = nc + Y * nh + X * nw;
	nbl = nc - Y * nh - X * nw;
	nbr = nc - Y * nh + X * nw;

	ftl = fc + Y * fh - X * fw;
	ftr = fc + Y * fh + X * fw;
	fbl = fc - Y * fh - X * fw;
	fbr = fc - Y * fh + X * fw;

	pl[TOP].set3Points(ntr,ntl,ftl);
	pl[BOTTOM].set3Points(nbl,nbr,fbr);
	pl[LEFT].set3Points(ntl,nbl,fbl);
	pl[RIGHT].set3Points(nbr,ntr,fbr);
	pl[NEARP].set3Points(ntl,ntr,nbr);
	pl[FARP].set3Points(ftr,ftl,fbl);
}

void CFrustum::setSpeedUp()
{
	speed += 3.0f;
}

void CFrustum::setSpeedDown()
{
	speed -= 3.0f;
	if( speed < 0.0f )
		speed = 3.0f;
}

//int CFrustum::pointInFrustum(glm::vec3 &p) {
//
//	float pcz,pcx,pcy,aux;
//
//	// compute vector from camera position to p
//	glm::vec3 v = p-cc;
//
//	pcz = glm::dot(v,Z);
//	if (pcz > farD || pcz < nearD)
// 		return(OUTSIDE);
//
// 	// compute and test the Y coordinate
// 	pcy = glm::dot(v,Y);
// 	aux = pcz * tang;
// 	if (pcy > aux || pcy < -aux)
// 		return(OUTSIDE);
//
// 	// compute and test the X coordinate
// 	pcx = glm::dot(v,X);
// 	aux = aux * ratio;
// 	if (pcx > aux || pcx < -aux)
//		return(OUTSIDE);
//
//	return(INSIDE);
//}


int CFrustum::pointInFrustum(glm::vec3 &p) 
{
	int result = INSIDE;
	for(int i=0; i < 6; i++) {

		if (pl[i].distance(p) < 0)
			return OUTSIDE;
	}
	return(result);
}

int CFrustum::sphereInFrustum(glm::vec3 &p, float raio) 
{
	int result = INSIDE;
	float distance;

	for(int i=0; i < 6; i++) {
		distance = pl[i].distance(p);
		if (distance < -raio)
			return OUTSIDE;
		else if (distance < raio)
			result =  INTERSECT;
	}
	return(result);
}

int CFrustum::AAboxInFrustum(glm::vec3 &min, glm::vec3 &max) 
{
	float v;
	int result = INSIDE;
	for(int i=0; i < 6; i++) {

		if ((v=pl[i].distance(getVertexP(pl[i].normal, min, max))) < 0)
			return OUTSIDE;
		else if ((v=pl[i].distance(getVertexN(pl[i].normal, min, max))) < 0)
			result = INTERSECT;
	}
	return(result);
}

int CFrustum::OBboxInFrustum(glm::vec3 &min, glm::vec3 &max, glm::mat4 &mInv)
{
	float v;
	int result = INSIDE;
	for(int i=0; i < 6; i++) {
		//transform plane normal to Box axis
		pl[i].set2BoxAxis(mInv);
		if ((v=pl[i].distanceT(getVertexP(pl[i].normalT, min, max))) < 0)
			return OUTSIDE;
		else if ((v=pl[i].distanceT(getVertexN(pl[i].normalT, min, max))) < 0)
			result = INTERSECT;
	}
	return(result);
}

glm::vec3 CFrustum::getVertexP(glm::vec3 &normal, glm::vec3 &min, glm::vec3 &max)
{
	glm::vec3 p;
	p.x = min.x;
	p.y = min.y;
	p.z = min.z;
	//if (normal.x <= 0)
	//	p.x = max.x;
	//if (normal.y <=0)
	//	p.y = max.y;
	if( normal.x >=0 )
		p.x = max.x;
	if( normal.y >=0 )
		p.y = max.y;
	if (normal.z >= 0)
		p.z = max.z;

	return p;
}

glm::vec3 CFrustum::getVertexN(glm::vec3 &normal, glm::vec3 &min, glm::vec3 &max)
{
	glm::vec3 n;
	n.x = max.x;
	n.y = max.y;
	n.z = max.z;
	//if (normal.x <= 0)
	//	n.x = min.x;
	//if (normal.y <=0)
	//	n.y = min.y;

	if (normal.x >= 0)
		n.x = min.x;
	if (normal.y >=0)
		n.y = min.y;
	if (normal.z >= 0)
		n.z = min.z;

	return n;
}

glm::mat4 CFrustum::getViewMatrix()
{
	return ViewMatrix;
}

glm::mat4 CFrustum::getProjectionMatrix()
{
	return ProjectionMatrix;
}

void CFrustum::computeMatricesFromInputs(GLFWwindow* window, int screenWidth, int screenHeight)
{
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	if(isFocus) {
		glfwGetCursorPos(window, &xpos, &ypos);

		// Reset mouse position for next frame
		glfwSetCursorPos(window, screenWidth /2, screenHeight /2);
	}
	// Compute new orientation
	if(!isDualCamera) {
		//eye/camera use same view
		horizontalAngle -= mouseSpeed * float(screenWidth /2 - xpos );
		verticalAngle   -= mouseSpeed * float(screenHeight /2 - ypos );
		hAT = horizontalAngle;
		vAT = verticalAngle;
	}
	else {
		if(isEye) {
			hAT -= mouseSpeed * float(screenWidth /2 - xpos );
			vAT -= mouseSpeed * float(screenHeight /2 - ypos );
		}
		else {
			horizontalAngle -= mouseSpeed * float(screenWidth /2 - xpos );
			verticalAngle   -= mouseSpeed * float(screenHeight /2 - ypos );
	//		horizontalAngle += mouseSpeed * float(screenWidth/2 - xpos );
	//		verticalAngle   += mouseSpeed * float( screenHeight/2 - ypos );
		}
	}
	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14159265358979323846f/2.0f), 
		0,
		cos(horizontalAngle - 3.14159265358979323846f/2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross( direction, right );
//	glm::vec3 up = glm::cross( right, direction );


	glm::vec3 directionT(
		cos(vAT) * sin(hAT), 
		sin(vAT),
		cos(vAT) * cos(hAT)
	);
	
	// Right vector
	glm::vec3 rightT = glm::vec3(
		sin(hAT - 3.14159265358979323846f/2.0f), 
		0,
		cos(hAT - 3.14159265358979323846f/2.0f)
	);

	// Up vector
	glm::vec3 upT = glm::cross( directionT, rightT );

	//frustum culling can depend on positionT or position (Dual)
	//but what is display by the shader is dependent on the position
	if(isDualCamera)
		setCamDef(positionT, positionT+directionT, upT);
	else
		setCamDef(position, position+direction, up);

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		if(!isDualCamera) {
			position += direction * deltaTime * speed;
			positionT = position;
		}
		else {
			if(isEye)
				positionT += directionT * deltaTime * speed;
			else
				position += direction * deltaTime * speed;
		}
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		if(!isDualCamera) {
			position -= direction * deltaTime * speed;
			positionT = position;
		}
		else {
			if(isEye)
				positionT -= directionT * deltaTime * speed;
			else
				position -= direction * deltaTime * speed;
		}
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		if(!isDualCamera) {
			position -= right * deltaTime * speed;
			positionT = position;
		}
		else {
			if(isEye)
				positionT -= rightT * deltaTime * speed;
			else
				position -= right * deltaTime * speed;
			//position += right * deltaTime * speed;
		}
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		if(!isDualCamera) {
			position += right * deltaTime * speed;
			positionT = position;
		}
		else {
			if(isEye)
				positionT += rightT * deltaTime * speed;
			else
				position += right * deltaTime * speed;
			//position -= right * deltaTime * speed;
		}
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	//Projection/View Matrix used by shader
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//this is different from the frustum parameter, this define what the shader will limit its drawing distance
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 1000.0f);
	// Camera matrix, ensure camera is near the nearPlane, so that mesh doesnt pop in-out in view
	glm::vec3 pos = position + direction * 1.2f;
	ViewMatrix       = glm::lookAt(
								pos,           // Camera is here
								pos+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}
