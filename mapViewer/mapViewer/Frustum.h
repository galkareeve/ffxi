#pragma once
#include <glm/glm.hpp>
#include "myEnum.h"
#include "Plane.h"

struct SFrustumFlg
{
	unsigned int left_out:1;
	unsigned int right_out:1;
	unsigned int top_out:1;
	unsigned int bottom_out:1;
	unsigned int near_out:1;
	unsigned int far_out:1;
	unsigned int inside:1;
	unsigned int outside:1;
	unsigned int partial:1;
};

struct GLFWwindow;
class CFrustum
{
public:
	CFrustum(void);
	~CFrustum(void);

	Plane pl[6];
	glm::vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
	float nearD, farD, ratio, angle,tang;
	float nw,nh,fw,fh;

	void setFrustumPos(float extend);
	void setFocus(bool f) { isFocus=f; }
	void setCamInternals(float angle, float ratio, float nearD, float farD);
	void setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u);
	void setSpeedUp();
	void setSpeedDown();
	float getSpeed() { return speed; };

	int pointInFrustum(glm::vec3 &p);
	int sphereInFrustum(glm::vec3 &p, float raio);
	int AAboxInFrustum(glm::vec3 &min, glm::vec3 &max);
	int OBboxInFrustum(glm::vec3 &min, glm::vec3 &max, glm::mat4 &mInv);
	glm::vec3 getVertexP(glm::vec3 &normal, glm::vec3 &min, glm::vec3 &max);
	glm::vec3 getVertexN(glm::vec3 &normal, glm::vec3 &min, glm::vec3 &max);

	void computeMatricesFromInputs(GLFWwindow* window, int screenWidth, int screenHeight);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	bool isUseDualCamera() { return isDualCamera; }
	void toggleCamera() { isDualCamera =! isDualCamera; if(!isDualCamera) isEye=false;}
	bool isMainEye() { return isEye; }
	void toggleEye() { isEye =! isEye; }

	glm::vec3 cc; // camera position
	glm::vec3 X,Y,Z; // the camera referential
	
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 PVM;

	// Initial position : on +Z
	glm::vec3 position, positionT; 
	// Initial horizontal angle : toward -Z
	float horizontalAngle, hAT;
	// Initial vertical angle : none
	float verticalAngle, vAT;
	// Initial Field of View
	float initialFoV;

	float speed; // 3 units / second
	float mouseSpeed;
	double xpos, ypos;
	//whether window have focus, so it doesnt spin when oof
	bool isFocus;
	bool isDualCamera;
	bool isEye;			//eye is where frustum culling,  camera is for shader

private:
	enum {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		NEARP,
		FARP
	};

};

