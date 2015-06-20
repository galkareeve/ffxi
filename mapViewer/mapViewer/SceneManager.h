#pragma once
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "myEnum.h"

class IDriver;
class ISceneNode;
class CTimer;
class IMesh;
class CFrustum;
class IMeshBuffer;
class CSceneManager
{
public:
	CSceneManager(void);
	~CSceneManager(void);

	void init();
	void addDriver(IDriver *in) { m_pDriver=in; }
	void addFrustum(CFrustum *in) { m_pFrustum=in; }
	void addSceneNode(ISceneNode *in) { m_solid.push_back(in); }
	void resetScene();
	IMesh* loadMesh(std::string fn, unsigned int tid);
	IMesh* loadMeshLandscape(std::string fn, unsigned int tid);
	ISceneNode* createSceneNode(IMesh*);
	ISceneNode* createSceneNodeLandscape(IMesh*);
	unsigned int getTime();
	void drawAll( glm::mat4 &Pmat, glm::mat4 &Vmat );
//	E_POS isPointInFrustum(glm::vec3 pt);
	int isAABoundingBoxInFrustum(glm::vec3 min, glm::vec3 max);
	int isOBBoundingBoxInFrustum(glm::vec3 min, glm::vec3 max, glm::mat4 &mInv);

	void populateFrustumPlane(IMeshBuffer *m_pFrustumMB);
	bool checkDependency(unsigned int fno, std::string &dependStr);

protected:
	IDriver *m_pDriver;
	std::vector<ISceneNode*> m_solid;
	CTimer *m_pTimer;
	CFrustum *m_pFrustum;
	std::map<unsigned int,std::string> m_mapDatDependency;
};

