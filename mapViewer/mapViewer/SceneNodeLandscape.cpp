#include "SceneNodeLandscape.h"
#include "OpenGLDriver.h"
#include "FFXILandscapeMesh.h"
#include "IMeshBuffer.h"
#include "MeshBufferGroup.h"
#include <iostream>
#include "Frustum.h"

CSceneNodeLandscape::CSceneNodeLandscape(ISceneNode *parent, CSceneManager *mgr) : ISceneNode(parent,mgr)
{
	m_curMMB=-1;
	m_curFrame=0;
	m_lastTime=0;
	m_pMesh=nullptr;
	m_isMZB=true;
	m_lastCount=-1;
	m_isOctree=true;
	m_drawCube=false;
	m_pCubeMB=nullptr;
	m_pFrustumMB=nullptr;
}

CSceneNodeLandscape::~CSceneNodeLandscape(void)
{
	m_pMesh->drop();
	delete m_pCubeMB;
	delete m_pFrustumMB;
	delete m_pOctree;
}

void CSceneNodeLandscape::addMesh(IMesh *in)
{ 
	m_pMesh = reinterpret_cast<CFFXILandscapeMesh*>(in);
	m_pMesh->grab();

	std::vector<CMeshBufferGroup*> vecMeshBufferGroup;
	//create Octree
	m_pMesh->getMeshBufferGroup(vecMeshBufferGroup);
	m_pOctree = new COctree;
	m_pOctree->Create(vecMeshBufferGroup, 3);

	//NOTE: all meshBufferGroup is added to the ROOT NODE!!!!
	//int mbIndex, meshCount = m_pMesh->getMeshBufferGroupCount();
	//std::vector<int> vecMeshReference;
	//for(int i=0; i<meshCount; ++i)
	//	vecMeshReference.push_back(0);

	////verify all mesh is being reference by the node
	//std::vector<OCT_NODE*> vecOctNode;
	//std::vector<int> vecMeshBufferIndexPtr;
	//m_pOctree->GetTables(vecOctNode, vecMeshBufferIndexPtr);
	//for(auto it=vecOctNode.begin(); it!=vecOctNode.end(); ++it) {
	//	for(int i=0,j=(*it)->meshBufferIndexStart; i<(*it)->meshBufferIndexCount; ++i,++j) {
	//		mbIndex=vecMeshBufferIndexPtr[j];
	//		vecMeshReference[mbIndex]=1;
	//	}
	//}

	//for(auto it=vecMeshReference.begin(); it!=vecMeshReference.end(); ++it) {
	//	if( (*it)==0 )
	//		std::cout << "mbIndex: " << (*it) << " not reference" << std::endl;
	//}

	//create a MeshBuffer for rendering cube
	m_pCubeMB = new IMeshBuffer(E_LINE);
	m_pCubeMB->generateFrameBuffer(1);
	m_pFrustumMB = new IMeshBuffer(E_LINE_LOOP);
	m_pFrustumMB->generateFrameBuffer(4);	//left, right, top, bottom using LINE_LOOP
}

bool CSceneNodeLandscape::animate(int frame)
{
	if(!m_pMesh)
		return false;

	return true;
}

void CSceneNodeLandscape::draw(IDriver *dr, glm::mat4 &ProjectionMatrix, glm::mat4 &ViewMatrix )
{
	//generate the Model Matrix
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	dr->initProjectionMatrix(ModelMatrix, ViewMatrix, MVP);

	if(m_isOctree) {
		drawOctree(dr);
		if(m_drawCube)
			drawCube(dr, MVP);
	}
	else {
		drawNonOctree(dr);
		if(m_curMMB!=-1) {
			drawCube(dr, MVP);
		}
	}
}

void CSceneNodeLandscape::drawNonOctree(IDriver *dr )
{
	int i,gc = m_pMesh->getMeshBufferGroupCount(),count=0,mbc;
	for(i=0; i<gc; ++i) {
		CMeshBufferGroup *pMBG = m_pMesh->getMeshBufferGroup(i);
		mbc=pMBG->getMeshBufferCount();
		for(int j=0; j<mbc; ++j) {
			IMeshBuffer *mb = pMBG->getMeshBuffer(j);
			dr->draw(floor(m_curFrame), mb);
		}
		count++;
	}

	if(count!=m_lastCount) {
		m_lastCount=count;
		std::cout << "MBG display: " << count << std::endl;
	}
}

void CSceneNodeLandscape::drawOctree(IDriver *dr )
{
	int i,gc = m_visibleMBG.size(),count=0,mbc;

	for(i=0; i<gc; ++i) {
		CMeshBufferGroup *pMBG = m_visibleMBG[i];
		if(pMBG->m_timeLastDrawn==m_lastTime)
			continue;

		pMBG->m_timeLastDrawn=m_lastTime;
		mbc=pMBG->getMeshBufferCount();
		for(int j=0; j<mbc; ++j) {
			IMeshBuffer *mb = pMBG->getMeshBuffer(j);			
			dr->draw(floor(m_curFrame), mb);
		}
		count++;
	}
	if(count!=m_lastCount) {
		m_lastCount=count;
		std::cout << "MBG display: " << count << std::endl;
	}
}

void CSceneNodeLandscape::drawCube(IDriver *dr, glm::mat4 &MVP)
{
	GLuint drID = dr->selectProgramID(1);
	GLuint MVPID = glGetUniformLocation(drID, "MVP");
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
	dr->drawCube(0, m_pCubeMB);
	if (m_curMMB == -1) {
		dr->drawCube(0, m_pFrustumMB);
		dr->drawCube(1, m_pFrustumMB);
		dr->drawCube(2, m_pFrustumMB);
		dr->drawCube(3, m_pFrustumMB);
	}
	dr->selectProgramID(0);
}

void CSceneNodeLandscape::setCurrentFrame(float frame)
{
	m_curFrame=frame;
}

void CSceneNodeLandscape::onAnimate(unsigned int timeMs)
{
	m_lastTime++;
	if(m_lastTime==10000)
		m_lastTime=0;

//	m_lastTime=timeMs;

	if(m_curMMB!=-1)
		return;

	//loop thru Octree and find the node within the frustum
	std::vector<OCT_NODE*> vecOctNode;
	std::vector<int> vecMeshBufferIndexPtr;
	m_pOctree->GetTables(vecOctNode, vecMeshBufferIndexPtr);	

	m_visibleMBG.clear();
	if(m_drawCube) {
		m_pCubeMB->clear();
		m_pFrustumMB->clear();
		//get frustum plane
		m_SceneManager->populateFrustumPlane(m_pFrustumMB);
	}
	m_checkedMB.clear();
	traverseChild(vecOctNode, vecMeshBufferIndexPtr, 0);
}

void CSceneNodeLandscape::traverseChild(std::vector<OCT_NODE*> &in, std::vector<int> &inIndex, int pos)
{
	int mbIndex=0,mzb;
	CMeshBufferGroup *pMBG;
	OCT_NODE *pnode = in[pos];
	if(pnode->meshBufferIndexCount==0)
		return;

	if(pnode->ChildIdx[0]==NULL_NODE) {
		//if this is leaf node, and no children, ==> its parent is PARTIAL
		checkMBGInFrustum(pnode->meshBufferIndexStart, pnode->meshBufferIndexCount, inIndex);
		return;
	}

//	OVERLAP ret;
	int ret;
	BoundingBox bbox;
	bbox=pnode->BBox;
	//Axis-Align Box
	if( (ret=m_SceneManager->isAABoundingBoxInFrustum( glm::vec3(bbox.x1, bbox.y1, bbox.z1), glm::vec3(bbox.x2, bbox.y2, bbox.z2)))==INSIDE) {
		for(int j=0, k=pnode->meshBufferIndexStart; j<pnode->meshBufferIndexCount; ++j, ++k) {
			mbIndex=inIndex[k];
			auto mit=m_checkedMB.find(mbIndex);
			if(mit!=m_checkedMB.end())
				continue;
			m_checkedMB.insert(std::pair<int,int>(mbIndex,1));
			m_visibleMBG.push_back(m_pMesh->getMeshBufferGroup(mbIndex));
		}
		if(m_drawCube)
			addColor2Cube(0.98f, pnode->BBox, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if(ret==INTERSECT) {
		if(m_drawCube)
			addColor2Cube(1.0f, pnode->BBox, glm::vec3(0.0f, 1.0f, 0.0f));

		for(int k=0; k<NUM_CHILDREN; ++k)
			if(pnode->ChildIdx[k]!=NULL_NODE)
				traverseChild(in, inIndex, pnode->ChildIdx[k]);

		//check individual MBG against the Frustum, if it hasnt been added
		checkMBGInFrustum(pnode->meshBufferIndexStart, pnode->meshBufferIndexCount, inIndex);
	}
	else {
		if(m_drawCube)
			addColor2Cube(0.98f, pnode->BBox, glm::vec3(0.3f, 0.3f, 1.f));
	}
}

void CSceneNodeLandscape::checkMBGInFrustum(int start, int count, std::vector<int> &inIndex)
{
	CMeshBufferGroup *pMBG;
	for(int j=0, k=start; j<count; ++j, ++k) {
		auto mit=m_checkedMB.find(inIndex[k]);
		if(mit!=m_checkedMB.end())
			continue;

		m_checkedMB.insert(std::pair<int,int>(inIndex[k],1));
		pMBG=m_pMesh->getMeshBufferGroup(inIndex[k]);
		if (pMBG == nullptr)
			continue;
		//Oriented Box
		if(m_SceneManager->isAABoundingBoxInFrustum(pMBG->m_minBoundRect, pMBG->m_maxBoundRect)!=OUTSIDE)
			m_visibleMBG.push_back(pMBG);
	}
}

void CSceneNodeLandscape::addColor2Cube( float per, BoundingBox &bbox, glm::vec3 &color) 
{
	BoundingBox box = bbox;
	if(per!=1.0f) {
		box.x1*=per;
		box.x2*=per;
		box.y1*=per;
		box.y2*=per;
		box.z1*=per;
		box.z2*=per;
	}
	//add the node's box to meshBuffer, draw 12 edge
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y1, box.z1));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y1, box.z2));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y1, box.z1));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y2, box.z1));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y1, box.z1));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y1, box.z1));

	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y2, box.z2));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y2, box.z1));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y2, box.z2));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y1, box.z2));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y2, box.z2));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y2, box.z2));

	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y2, box.z1));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y2, box.z1));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y2, box.z1));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y1, box.z1));

	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y2, box.z2));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y2, box.z1));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y2, box.z2));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y1, box.z2));

	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y1, box.z2));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y1, box.z1));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x2, box.y1, box.z2));
	m_pCubeMB->addVertexBuffer(0, glm::vec3(box.x1, box.y1, box.z2));

	//add 12 edge == 24 vertice
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
	m_pCubeMB->addNormalBuffer(0, color);
}

void CSceneNodeLandscape::setCurrentMMB(int mmb)
{
	int maxC=getMaxCount();
	if(mmb!=-1) {
		if(mmb>=maxC || mmb<0)
			mmb=0;
	}
	m_curMMB=mmb;
	m_pMesh->refreshMeshBufferGroup(m_curMMB, m_isMZB);
	if(mmb!=-1)
		updateBoundingRect();

}

void CSceneNodeLandscape::nextMMB()
{
	if(m_isOctree) {
		std::cout << "using octree, no MMB, press O to remove octree" << std::endl;
		return;
	}
	int maxC=getMaxCount();
	if(m_curMMB==-1)
		m_curMMB=0;
	else {
		m_curMMB++;
		if(m_curMMB>=maxC)
			m_curMMB=0;
	}
	m_pMesh->refreshMeshBufferGroup(m_curMMB, m_isMZB);
	updateBoundingRect();
}

void CSceneNodeLandscape::prevMMB()
{
	if(m_isOctree)
		return;

	int maxC=getMaxCount();
	if(m_curMMB==-1)
		m_curMMB=0;
	else {
		m_curMMB--;
		if(m_curMMB<0)
			m_curMMB=maxC-1;
	}
	m_pMesh->refreshMeshBufferGroup(m_curMMB, m_isMZB);
	updateBoundingRect();
}

void CSceneNodeLandscape::wirteMeshBuffer()
{
	m_pMesh->writeMeshInfo(m_curFrame, m_curMMB);
}

int CSceneNodeLandscape::getMaxCount()
{
	if(m_isMZB)
		return m_pMesh->getB100count();
	
	return m_pMesh->getMMBCount();
}

void CSceneNodeLandscape::toggleIsOctree() 
{
	m_isOctree=!m_isOctree;
	if(m_isOctree) {
		m_curMMB=-1;
		m_isMZB=true;
		m_visibleMBG.clear();
	}
	else
		m_curMMB=0;

	m_pMesh->refreshMeshBufferGroup(m_curMMB, m_isMZB);
	if(!m_isOctree && m_curMMB!=-1) {
		updateBoundingRect();
	}
}

void CSceneNodeLandscape::updateBoundingRect()
{
	BoundingBox bbox;
	//clear vertices/color
	m_pCubeMB->clear();

	int i,c = m_pMesh->getMeshBufferGroupCount();
	for(i=0; i<c; ++i) {
		CMeshBufferGroup *pMBG = m_pMesh->getMeshBufferGroup(i);
		bbox.x1 = pMBG->m_minBoundRect.x;
		bbox.y1 = pMBG->m_minBoundRect.y;
		bbox.z1 = pMBG->m_minBoundRect.z;
		bbox.x2 = pMBG->m_maxBoundRect.x;
		bbox.y2 = pMBG->m_maxBoundRect.y;
		bbox.z2 = pMBG->m_maxBoundRect.z;
		addColor2Cube(1.0f, bbox, glm::vec3(0.0f, 1.0f, 0.0f));
	}
}

float CSceneNodeLandscape::getExtend()
{
	return m_pOctree->getExtend();
}
