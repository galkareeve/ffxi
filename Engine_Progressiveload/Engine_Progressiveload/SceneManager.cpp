#include "SceneManager.h"

#include "FFXIMesh.h"
#include "SceneNode.h"
#include "Timer.h"

CSceneManager::CSceneManager(void)
{
	m_pTimer = new CTimer;
	m_pTimer->initVirtualTimer();
}


CSceneManager::~CSceneManager(void)
{
}

IMesh* CSceneManager::loadMesh(std::string fn)
{
	CFFXIMesh *ffmesh = new CFFXIMesh(m_pDriver);
	if( ffmesh->loadModelFile(fn) ) {
		ffmesh->prepareFrameBuffer();
		return ffmesh;
	}
	return NULL;
}

ISceneNode* CSceneManager::createSceneNode(IMesh *mesh)
{
	CSceneNode *node = new CSceneNode(0);
	node->addMesh(mesh);
	m_solid.push_back(node);
	
	return node;
}

void CSceneManager::drawAll(glm::mat4 &Pmat, glm::mat4 &Vmat)
{
	m_pTimer->tick();
	unsigned int t=m_pTimer->getTime();
	for(auto it=m_solid.begin(); it!=m_solid.end(); ++it) {
		(*it)->onAnimate(t);
		(*it)->draw(m_pDriver, Pmat, Vmat);
	}
}

unsigned int CSceneManager::getTime()
{
	return m_pTimer->getTime();
}