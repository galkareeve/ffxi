#pragma once
#include <vector>
#include <glm/glm.hpp>

class IDriver;
class ISceneNode;
class CTimer;
class IMesh;
class CSceneManager
{
public:
	CSceneManager(void);
	~CSceneManager(void);

	void addDriver(IDriver *in) { m_pDriver=in; }
	void addSceneNode(ISceneNode *in) { m_solid.push_back(in); }
	IMesh* loadMesh(std::string fn);
	ISceneNode* createSceneNode(IMesh*);
	unsigned int getTime();
	void drawAll( glm::mat4 &Pmat, glm::mat4 &Vmat );

protected:
	IDriver *m_pDriver;
	std::vector<ISceneNode*> m_solid;
	CTimer *m_pTimer;
};

