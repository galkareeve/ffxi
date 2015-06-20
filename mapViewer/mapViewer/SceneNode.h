#pragma once
#include "iscenenode.h"

class IMesh;
class CSceneManager;
class CSceneNode :	public ISceneNode
{
public:
	CSceneNode(ISceneNode *parent, CSceneManager *mgr);
	virtual ~CSceneNode(void);

	bool animate(int frame);
	void draw(IDriver *dr, glm::mat4 &Pmat, glm::mat4 &Vmat );
	void addMesh(IMesh *in) { m_pMesh = in; }
	IMesh* getMesh() { return m_pMesh; }

	float getCurrentFrame() { return m_curFrame; }
	void setCurrentFrame(float frame);
	bool setFrameLoop(int begin, int end);
	void setAnimationSpeed(float framesPerSecond);
	float getAnimationSpeed();

	void onAnimate(unsigned int timeMS);
	void buildFrameNr(unsigned int timeMS);

//	void setPosition(glm::vec3 pos) {m_position=pos;}
//	glm::vec3 getPosition() { return m_position; }

protected:
	
	IMesh *m_pMesh;
	int m_startFrame;
	int m_endFrame;
	float m_curFrame;
	float m_fps;

	unsigned int m_lastTime;
//	glm::vec3 m_position;
};

