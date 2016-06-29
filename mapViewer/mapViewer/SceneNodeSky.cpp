#include "SceneNodeSky.h"
#include "FFXILandscapeMesh.h"
#include "IMeshBuffer.h"
#include "MeshBufferGroup.h"
#include "OpenGLDriver.h"
#include <glm/gtc/type_ptr.hpp>

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
	COpenGLDriver *oldr = reinterpret_cast<COpenGLDriver*>(dr);
	GLuint drID = oldr->selectProgramID(2);
	GLuint shaderTextureID = glGetUniformLocation(drID, "myTextureSampler");
	oldr->setShaderTextureID(shaderTextureID);

	glm::mat4 MVP = Pmat * Vmat;
	GLuint MVPID = glGetUniformLocation(drID, "MVP");
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, glm::value_ptr(MVP));
	GLuint MVID = glGetUniformLocation(drID, "MV");
	glUniformMatrix4fv(MVID, 1, GL_FALSE, glm::value_ptr(Vmat));
	GLuint PID = glGetUniformLocation(drID, "P");
	glUniformMatrix4fv(PID, 1, GL_FALSE, glm::value_ptr(Pmat));

	glm::vec3 skyColor(0.3f, 0.5f, 0.7f);
	GLuint skyColorID = glGetUniformLocation(drID, "skyColor");
	glUniform3fv(skyColorID, 1, glm::value_ptr(skyColor));

	int mbc = m_pSkyMBG->getMeshBufferCount();
	for (int j = 0; j<mbc; ++j) {
		IMeshBuffer *mb = m_pSkyMBG->getMeshBuffer(j);
//		if (mb->m_useAlpha && (mb->getMultipler() & 0x08))
//			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		else
			glBlendFunc(GL_ONE, GL_ZERO);
			oldr->draw(0, mb, mb->getMultipler(), mb->m_useAlpha);
	}

	oldr->selectProgramID(0);
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