#include "SceneNodeSky.h"
#include "FFXILandscapeMesh.h"
#include "IMeshBuffer.h"
#include "MeshBufferGroup.h"
#include "OpenGLDriver.h"

CSceneNodeSky::CSceneNodeSky(ISceneNode *parent, CSceneManager *mgr) : ISceneNode(parent, mgr)
{
	m_pSkyMBG = nullptr;
}

CSceneNodeSky::~CSceneNodeSky()
{
	delete m_pSkyMBG;
}

bool CSceneNodeSky::animate(int frame)
{
	return true;
}

void CSceneNodeSky::draw(IDriver *dr, glm::mat4 &Pmat, glm::mat4 &Vmat)
{
	int mbc = m_pSkyMBG->getMeshBufferCount();
	for (int j = 0; j<mbc; ++j) {
		IMeshBuffer *mb = m_pSkyMBG->getMeshBuffer(j);
		if (mb->m_useAlpha && (mb->getMultipler() & 0x08))
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		else
			glBlendFunc(GL_ONE, GL_ZERO);
		dr->draw(0, mb, mb->getMultipler(), mb->m_useAlpha);
	}
}

void CSceneNodeSky::addMesh(IMesh *in)
{

}

IMesh* CSceneNodeSky::getMesh()
{
	return nullptr;
}

void CSceneNodeSky::onAnimate(unsigned int timeMs)
{
	//move the cloud...TBC
}

void CSceneNodeSky::createSky(IMesh *pMesh)
{
	CFFXILandscapeMesh *pFFLandMesh = reinterpret_cast<CFFXILandscapeMesh*>(pMesh);
	m_pSkyMBG = pFFLandMesh->generateCloudMeshBuffer();

}