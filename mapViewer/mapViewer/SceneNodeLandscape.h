#pragma once
#include "iscenenode.h"
#include "Octree.h"
#include <map>

class CFFXILandscapeMesh;
class COctree;
class CLooseTree;
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

	void addMesh(IMesh *in);
	IMesh* getMesh() { return (IMesh*)m_pMesh; }

	int getCurrentFrame() { return m_curFrame; }
	void setCurrentFrame(int frame);
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
	void toggleDrawPVS();
	void toggleDrawNormal();
	bool isDrawPVS() { return m_drawPVS; }
	int getCurrentPVS() { return m_curPVS; }
	void nextPVS();
	void prevPVS();

	bool isOctree() { return m_isOctree; }
	bool isMZB() { return m_isMZB; }
	bool isDrawCube() { return m_drawCube; }
	int getMaxCount();
	void traverseChild(std::vector<OCT_NODE*> &in, std::vector<int> &inIndex, int k);
	void checkMBGInFrustum(int start, int count, std::vector<int> &inIndex);
	void updateBoundingRect();
	float getExtend();
	void traverseNode(unsigned int addrID);
	void populateNormal(IMeshBuffer *mb, int frame);

protected:
	void addColor2Cube( float per, BoundingBox &bbox, glm::vec3 &in);

	CFFXILandscapeMesh *m_pMesh;
	COctree *m_pOctree;
	CLooseTree *m_pLooseTree;

	unsigned int m_lastTime;
	int m_curFrame;
	int m_curMMB;
	int m_curPVS;

	bool m_isMZB;		//toggle btw lookupMMB by B100 or MMB index
	bool m_isOctree;
	bool m_drawCube;
	bool m_isMMBModelInc;
	bool m_drawPVS;
	bool m_drawNormal;
	int m_curMMBModel;	//draw individual model within each MMB

	IMeshBuffer *m_pCubeMB;
	IMeshBuffer *m_pFrustumMB;
	IMeshBuffer *m_pNormalMB;

	int m_lastCount;
	std::vector<CMeshBufferGroup*> m_visibleMBG;
	std::map<int, int> m_checkedMB;
};

