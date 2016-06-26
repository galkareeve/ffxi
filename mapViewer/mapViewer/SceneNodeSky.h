#pragma once
#include "ISceneNode.h"

class CMeshBufferGroup;
class CSceneNodeSky :	public ISceneNode
{
public:
	CSceneNodeSky(ISceneNode *parent, CSceneManager *mgr);
	virtual ~CSceneNodeSky();

	virtual bool animate(int frame);
	virtual void draw(IDriver *dr, glm::mat4 &Pmat, glm::mat4 &Vmat);
	virtual void addMesh(IMesh *in);
	virtual IMesh* getMesh();
	virtual void onAnimate(unsigned int timeMs);

	void createSky(IMesh *pMesh);

protected:
	CMeshBufferGroup *m_pSkyMBG;
};

