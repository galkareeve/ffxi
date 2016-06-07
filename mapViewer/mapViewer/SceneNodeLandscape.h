#pragma once
#include "iscenenode.h"
#include "Octree.h"
#include <map>

class CFFXILandscapeMesh;
class COctree;
class IMeshBuffer;
class CMeshBufferGroup;
class CSceneNodeLandscape :	public ISceneNode
{
public:
	CSceneNodeLandscape(ISceneNode *parent, CSceneManager *mgr);
	virtual ~CSceneNodeLandscape(void);

	bool animate(int frame);
	void draw(IDriver *dr, glm::mat4 &Pmat, glm::mat4 &Vmat );
	void drawNonOctree(IDriver *dr );
	void drawOctree(IDriver *dr );
	void drawCube(IDriver *dr, glm::mat4 &MVP);
	void testDrawCube(IDriver *dr);

	void addMesh(IMesh *in);
	IMesh* getMesh() { return (IMesh*)m_pMesh; }

	float getCurrentFrame() { return m_curFrame; }
	void setCurrentFrame(float frame);
	void onAnimate(unsigned int timeMS);

	void nextMMB();
	void prevMMB();
	void nextMMBModel();
	int getMMBModelInfo() {	return m_curMMBModel; }
	void toggleMMBModelInclusive() { m_isMMBModelInc = !m_isMMBModelInc; }
	bool isMMBModelInclusive() { return m_isMMBModelInc; }
	void setCurrentMMB(int mmb);
	int getCurrentMMB() { return m_curMMB; }

	void wirteMeshBuffer();
	void toggleIsMZB() { m_isMZB=!m_isMZB; }
	void toggleIsOctree();
	void toggleDrawCube() { m_drawCube=!m_drawCube; }
	bool isOctree() { return m_isOctree; }
	bool isMZB() { return m_isMZB; }
	bool isDrawCube() { return m_drawCube; }
	int getMaxCount();
	void traverseChild(std::vector<OCT_NODE*> &in, std::vector<int> &inIndex, int k);
	void checkMBGInFrustum(int start, int count, std::vector<int> &inIndex);
	void updateBoundingRect();
	float getExtend();

protected:
	void addColor2Cube( float per, BoundingBox &bbox, glm::vec3 &in);

	CFFXILandscapeMesh *m_pMesh;
	COctree *m_pOctree;
	unsigned int m_lastTime;
	float m_curFrame;
	int m_curMMB;
	
	bool m_isMZB;		//toggle btw lookupMMB by B100 or MMB index
	bool m_isOctree;
	bool m_drawCube;
	bool m_isMMBModelInc;
	int m_curMMBModel;	//draw individual model within each MMB

	IMeshBuffer *m_pCubeMB;
	IMeshBuffer *m_pFrustumMB;

	int m_lastCount;
	std::vector<CMeshBufferGroup*> m_visibleMBG;
	std::map<int, int> m_checkedMB;
};

