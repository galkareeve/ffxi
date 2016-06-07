#include "SceneManager.h"

#include "FFXIMesh.h"
#include "SceneNode.h"
#include "FFXILandscapeMesh.h"
#include "SceneNodeLandscape.h"
#include "Timer.h"
#include "Frustum.h"
#include "IMeshBuffer.h"
#include <fstream>
#include <string>
#include "Utility.h"

CSceneManager::CSceneManager(void)
{
	m_pTimer = new CTimer;
	m_pTimer->initVirtualTimer();
	init();
}


CSceneManager::~CSceneManager(void)
{
	delete m_pTimer;
}

void CSceneManager::init()
{
	char buf[255];
	std::list<std::string> listStr;
	std::string datnoStr, dependStr;
	std::ifstream ifs;
	ifs.open("depend.txt");
	if(ifs.is_open()) {
		ifs.getline(buf,255);
		while(!ifs.eof()) {
			dependStr.assign(buf);
			datnoStr = CUtility::stripFirst(",", dependStr);
			m_mapDatDependency.insert(pair<unsigned int, std::string>(atoi(datnoStr.c_str()), dependStr));
			ifs.getline(buf,255);
		}
		ifs.close();
	}
}

void CSceneManager::resetScene()
{
	//remove all sceneNode and octree
	for(auto it=m_solid.begin(); it!=m_solid.end(); ++it) {
		delete (*it);
	}
	m_solid.clear();
}

IMesh* CSceneManager::loadMesh(std::string fn, unsigned int tid)
{
	CFFXIMesh *ffmesh = new CFFXIMesh(m_pDriver, tid);
	if( ffmesh->loadModelFile(fn) ) {
		ffmesh->prepareFrameBuffer();
		return ffmesh;
	}
	return NULL;
}

IMesh* CSceneManager::loadMeshLandscape(std::string fn, unsigned int tid)
{
	CFFXILandscapeMesh *ffmeshLandscape = new CFFXILandscapeMesh(m_pDriver, tid);
	if( ffmeshLandscape->loadModelFile(fn,this) ) {
		std::cout << "Loading Dat: " << fn << " ......" << std::endl;
		return ffmeshLandscape;
	}
	return NULL;
}

ISceneNode* CSceneManager::createSceneNode(IMesh *mesh)
{
	CSceneNode *node = new CSceneNode(0, this);
	node->addMesh(mesh);
	m_solid.push_back(node);
	
	return node;
}

ISceneNode* CSceneManager::createSceneNodeLandscape(IMesh *mesh)
{
	CSceneNodeLandscape *node = new CSceneNodeLandscape(0, this);
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

int CSceneManager::isAABoundingBoxInFrustum(glm::vec3 min, glm::vec3 max)
{
	return m_pFrustum->AAboxInFrustum(min, max);
}

int CSceneManager::isOBBoundingBoxInFrustum(glm::vec3 min, glm::vec3 max, glm::mat4 &mInv)
{
	return m_pFrustum->OBboxInFrustum(min, max, mInv);
}

void CSceneManager::populateFrustumPlane(IMeshBuffer *mb)
{
	//left plane frame 0
	mb->addVertexBuffer(0, m_pFrustum->ntl);
	mb->addVertexBuffer(0, m_pFrustum->nbl);
	mb->addVertexBuffer(0, m_pFrustum->fbl);
	mb->addVertexBuffer(0, m_pFrustum->ftl);
	mb->addNormalBuffer(0, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(0, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(0, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(0, glm::vec3(0.0f,0.0f,0.0f));
	//right plane frame 1
	mb->addVertexBuffer(1, m_pFrustum->nbr);
	mb->addVertexBuffer(1, m_pFrustum->ntr);
	mb->addVertexBuffer(1, m_pFrustum->ftr);
	mb->addVertexBuffer(1, m_pFrustum->fbr);
	mb->addNormalBuffer(1, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(1, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(1, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(1, glm::vec3(0.0f,0.0f,0.0f));
	//top plane frame 2
	mb->addVertexBuffer(2, m_pFrustum->ntl);
	mb->addVertexBuffer(2, m_pFrustum->ftl);
	mb->addVertexBuffer(2, m_pFrustum->ftr);
	mb->addVertexBuffer(2, m_pFrustum->ntr);
	mb->addNormalBuffer(2, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(2, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(2, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(2, glm::vec3(0.0f,0.0f,0.0f));
	//bottom plane frame 3
	mb->addVertexBuffer(3, m_pFrustum->nbl);
	mb->addVertexBuffer(3, m_pFrustum->nbr);
	mb->addVertexBuffer(3, m_pFrustum->fbr);
	mb->addVertexBuffer(3, m_pFrustum->fbl);
	mb->addNormalBuffer(3, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(3, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(3, glm::vec3(0.0f,0.0f,0.0f));
	mb->addNormalBuffer(3, glm::vec3(0.0f,0.0f,0.0f));
}

bool CSceneManager::checkDependency(unsigned int fno, std::string &dependStr)
{
	auto mit = m_mapDatDependency.find(fno);
	if(mit==m_mapDatDependency.end())
		return false;

	dependStr = mit->second;
	return true;
}
