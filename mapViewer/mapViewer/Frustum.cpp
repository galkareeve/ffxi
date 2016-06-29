#include "Frustum.h"
// Include GLFW
#include <glfw/glfw3.h>
//extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>

#define PI	3.14159265358979323846
#define ANG2RAD 3.14159265358979323846/180.0
//#define	CLIP_SPACE
#define GEOMETRIC

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
	this->angle = ANG2RAD*angle;
	this->nearD = nearD;
	this->farD = farD;

	// compute width and height of the near section
	tang = (float)tan(ANG2RAD * angle * 0.5) ;
	nh = nearD * tang;
	nw = nh * ratio; 
//	fh = farD  * tang;
	fh = 1000.0f * tang;		//cap the culling @ 1000.f
	fw = fh * ratio;
}

//call by computeMatricesFromInputs
void CFrustum::setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u) 
{
	cc = p;
//#ifdef GEOMETRIC
	glm::vec3 dir,nc,fc,X,Y,Z;

	Z = p - l;
	Z = glm::normalize(Z);

	X = glm::cross(u, Z);
	X = glm::normalize(X);

	Y = glm::cross(Z, X);

	nc = p - Z * nearD;
//	fc = p - Z * farD;
	fc = p - Z * 1000.0f;

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
//#endif

#ifdef CLIP_SPACE
	glm::mat4 view = glm::lookAt(p,l,u);
	PVM = ProjectionMatrix * view;
#endif
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
#ifdef GEOMETRIC
	//9 fps
	for(int i=0; i < 6; i++) {

		if ((v=pl[i].distance(getVertexP(pl[i].normal, min, max))) < 0)
			return OUTSIDE;
		else if ((v=pl[i].distance(getVertexN(pl[i].normal, min, max))) < 0)
			result = INTERSECT;
	}
	return(result);
#endif

#ifdef CLIP_SPACE
	//check the 8 corner of boundingBox within the screen space
	//-wc < xc < wc
	//-wc < yc < wc
	//-wc < zc < wc

	//glm is column major
	glm::vec4 row1 = glm::row(PVM,0);
	glm::vec4 row2 = glm::row(PVM, 1);
	glm::vec4 row3 = glm::row(PVM, 2);
	glm::vec4 row4 = glm::row(PVM, 3);

	std::vector<glm::vec4> vecPlane;
	vecPlane.push_back(glm::vec4((row1.x + row4.x), (row1.y+row4.y), (row1.z+row4.z), (row1.w+row4.w)));			//left
	vecPlane.push_back(glm::vec4((-row1.x + row4.x), (-row1.y + row4.y), (-row1.z + row4.z), (-row1.w + row4.w)));	//right
	vecPlane.push_back(glm::vec4((row2.x + row4.x), (row2.y + row4.y), (row2.z + row4.z), (row2.w + row4.w)));		//bottom
	vecPlane.push_back(glm::vec4((-row2.x + row4.x), (-row2.y + row4.y), (-row2.z + row4.z), (-row2.w + row4.w)));	//top
	vecPlane.push_back(glm::vec4((row3.x + row4.x), (row3.y + row4.y), (row3.z + row4.z), (row3.w + row4.w)));		//near
	vecPlane.push_back(glm::vec4((-row3.x + row4.x), (-row3.y + row4.y), (-row3.z + row4.z), (-row3.w + row4.w)));	//far

	int in=0, out = 0;

	//std::vector<glm::vec4> vecBox;
	//vecBox.push_back(glm::vec4(min.x, min.y, min.z, 1.0f));
	//vecBox.push_back(glm::vec4(min.x, min.y, max.z, 1.0f));
	//vecBox.push_back(glm::vec4(min.x, max.y, min.z, 1.0f));
	//vecBox.push_back(glm::vec4(min.x, max.y, max.z, 1.0f));
	//vecBox.push_back(glm::vec4(max.x, min.y, min.z, 1.0f));
	//vecBox.push_back(glm::vec4(max.x, min.y, max.z, 1.0f));
	//vecBox.push_back(glm::vec4(max.x, max.y, min.z, 1.0f));
	//vecBox.push_back(glm::vec4(max.x, max.y, max.z, 1.0f));

	//float xc, yc, zc, wc;
	////foreach plane, check 8 point
	//for (auto it = vecPlane.begin(); it != vecPlane.end(); ++it) {
	//	in = out = 0;
	//	for (auto bt = vecBox.begin(); bt != vecBox.end(); ++bt) {
	//		xc = (*it).x * (*bt).x;
	//		yc = (*it).y * (*bt).y;
	//		zc = (*it).z * (*bt).z;
	//		wc = (*it).w * (*bt).w;

	//		if (xc + yc + zc + wc > 0)
	//			in++;
	//		else
	//			out++;
	//	}

	//	//if all corners are out
	//	if (!in)
	//		return OUTSIDE;
	//	// if some corners are out and others are in
	//	else if (out)
	//		result = INTERSECT;
	//}

	//save 26 multiplication (4 -> 6 fps)
	float x1, x2, y1, y2, z1, z2, w;
	for (auto it = vecPlane.begin(); it != vecPlane.end(); ++it) {
		in = out = 0;
		x1 = (*it).x * min.x;
		x2 = (*it).x * max.x;

		y1 = (*it).y * min.y;
		y2 = (*it).y * max.y;

		z1 = (*it).z * min.z;
		z2 = (*it).z * max.z;

		w = (*it).w;
		
		//check 8 corner
		(x1 + y1 + z1 + w > 0) ? in++ : out++;
		(x1 + y1 + z2 + w > 0) ? in++ : out++;
		(x1 + y2 + z1 + w > 0) ? in++ : out++;
		(x1 + y2 + z2 + w > 0) ? in++ : out++;
		(x2 + y1 + z1 + w > 0) ? in++ : out++;
		(x2 + y1 + z2 + w > 0) ? in++ : out++;
		(x2 + y2 + z1 + w > 0) ? in++ : out++;
		(x2 + y2 + z2 + w > 0) ? in++ : out++;

		if (!in)
			return OUTSIDE;
		else if (out)
			result = INTERSECT;
	}
	return result;
#endif
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
		//This function returns the position of the cursor, in screen coordinates, relative to the upper-left corner of the client area of the specified window.
		glfwGetCursorPos(window, &xpos, &ypos);

		// Reset mouse position for next frame
		glfwSetCursorPos(window, screenWidth /2, screenHeight /2);
	}
	// Compute new orientation
	if(!isDualCamera) {
		//eye/camera use same view
		horizontalAngle -= mouseSpeed * float(screenWidth /2 - xpos );
		verticalAngle -= mouseSpeed * float(screenHeight / 2 - ypos);
		//horizontalAngle += mouseSpeed * float(screenWidth / 2 - xpos);
		//verticalAngle += mouseSpeed * float(screenHeight / 2 - ypos);

		hAT = horizontalAngle;
		vAT = verticalAngle;
	}
	else {
		if(isEye) {
			hAT -= mouseSpeed * float(screenWidth /2 - xpos );
			vAT -= mouseSpeed * float(screenHeight /2 - ypos );
			//hAT += mouseSpeed * float(screenWidth / 2 - xpos);
			//vAT += mouseSpeed * float(screenHeight / 2 - ypos);
		}
		else {
			horizontalAngle -= mouseSpeed * float(screenWidth /2 - xpos );
			verticalAngle -= mouseSpeed * float(screenHeight / 2 - ypos);
			//horizontalAngle += mouseSpeed * float(screenWidth/2 - xpos );
			//verticalAngle   += mouseSpeed * float( screenHeight/2 - ypos );			
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
		sin(horizontalAngle - PI /2.0f),
		0,
		cos(horizontalAngle - PI /2.0f)
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
		sin(hAT - PI /2.0f),
		0,
		cos(hAT - PI /2.0f)
	);

	// Up vector
	glm::vec3 upT = glm::cross( directionT, rightT );

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

	//frustum culling can depend on positionT or position (Dual)
	//but what is display by the shader is dependent on the position
	if (isDualCamera)
		setCamDef(positionT, positionT + directionT, upT);
	else
		setCamDef(position, position + direction, up);

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	//Projection/View Matrix used by shader
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//this is different from the frustum parameter, this define what the shader will limit its drawing distance
//	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 1000.0f);

	//glm::perspective (angle) is expresssed in Radians.
	ProjectionMatrix = glm::perspective(angle, ratio, nearD, farD);
	// Camera matrix, ensure camera is near the nearPlane, so that mesh doesnt pop in-out in view
//	glm::vec3 pos = position + direction * 1.2f;
	glm::vec3 pos = position + direction;
	ViewMatrix       = glm::lookAt(
								pos,           // Camera is here
								pos+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}
