#include "SceneNode.h"
#include "IMesh.h"
#include "IMeshBuffer.h"
#include "MeshBufferGroup.h"
#include "IDriver.h"

#include <glm/gtc/matrix_access.hpp>

CSceneNode::CSceneNode(ISceneNode *parent, CSceneManager *mgr) : ISceneNode(parent,mgr)
{
	m_pMesh=nullptr;
	m_lastTime=0;
	m_startFrame=0;
	m_endFrame=0;
	m_curFrame=0;
	m_fps=30;
//	m_position = glm::vec3(0,0,0);
}


CSceneNode::~CSceneNode(void)
{
}

bool CSceneNode::animate(int frame)
{
	if(!m_pMesh)
		return false;

	return m_pMesh->animate(frame);
}

void CSceneNode::draw(IDriver *dr, glm::mat4 &ProjectionMatrix, glm::mat4 &ViewMatrix )
{
	//generate the Model Matrix
	glm::mat4 ModelMatrix = glm::mat4(1.0);
//	ModelMatrix = glm::row(ModelMatrix, 3, glm::vec4(m_position,1));
	ModelMatrix = glm::column(ModelMatrix, 3, glm::vec4(m_Position,1));
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	dr->initProjectionMatrix(ModelMatrix, ViewMatrix, MVP);

	int i,mbc,gc = m_pMesh->getMeshBufferGroupCount();
	for(i=0; i<gc; ++i) {
		CMeshBufferGroup *pMBG=m_pMesh->getMeshBufferGroup(i);
		if (pMBG == nullptr)
			continue;
		mbc=pMBG->getMeshBufferCount();
		for(int j=0; j<mbc; ++j) {
			IMeshBuffer *mb = pMBG->getMeshBuffer(j);
			dr->draw(floor(m_curFrame), mb);
		}
	}
}

void CSceneNode::setCurrentFrame(float frame)
{
	m_curFrame=frame;
}

bool CSceneNode::setFrameLoop(int begin, int end)
{
	m_curFrame=m_startFrame=begin;
	m_endFrame=end;
	return true;
}

void CSceneNode::setAnimationSpeed(float framesPerSecond)
{
	m_fps=framesPerSecond*0.001f;
}

float CSceneNode::getAnimationSpeed()
{
	return m_fps;
}

void CSceneNode::onAnimate(unsigned int timeMs)
{
	if (m_lastTime==0)	// first frame
	{
		m_lastTime = timeMs;
	}

	// set CurrentFrameNr
	buildFrameNr(timeMs-m_lastTime);
	//when frame is not animated and it is lock, because other sceneNode is animating it, then set to frame 0, since it is always animated.
	if(!animate(m_curFrame))
		m_curFrame=0;
	m_lastTime=timeMs;

	ISceneNode::onAnimate(timeMs);
}

void CSceneNode::buildFrameNr(unsigned int timeMS)
{
//	char title[200];
	if(m_startFrame==m_endFrame)
		m_curFrame=m_startFrame;
	else {
		m_curFrame += timeMS * m_fps;
		//sprintf_s(title,200,"Start: %d, end: %d, frame: %f, timeMS: %d, fps: %f", m_startFrame, m_endFrame, m_curFrame, timeMS, m_fps);
		//glfwSetWindowTitle(window, title);
		if(m_curFrame > m_endFrame)
			m_curFrame = m_startFrame + fmod(m_curFrame - m_startFrame, float(m_endFrame-m_startFrame));
	}
}

