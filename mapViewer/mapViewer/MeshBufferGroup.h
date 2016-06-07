#pragma once
#include "IMeshBuffer.h"

//create a MeshBuffer group to hold the MeshBuffer, since the boundingRect is the same
//it will minimize frustum culling time.  Each MMB can contain multiple model (numModel)
//each instance of meshBufferGroup is a single MZB 

class CMeshBufferGroup
{
public:
	CMeshBufferGroup(int mzb, int mmb);
	~CMeshBufferGroup(void);

	IMeshBuffer* getMeshBuffer(unsigned int i);
	void getMeshBuffer(std::vector<IMeshBuffer*> &out) {out=m_meshBuffers;}
	void addMeshBuffer(IMeshBuffer *in) {m_meshBuffers.push_back(in); };
	int getMeshBufferCount() { return m_meshBuffers.size(); };

	int m_MZBIndex;
	int m_MMBIndex;
	unsigned int m_timeLastDrawn;	//mark this meshBuffer as drawn to avoid repeated drawing, due to mesh occupying multiple node in the Octree

	//BoundingRect (min,max), axis align after transform
	glm::vec3 m_minBoundRect;
	glm::vec3 m_maxBoundRect;

	//for frustum culling
	glm::vec3 m_origMinBoundRect;
	glm::vec3 m_origMaxBoundRect;

	std::vector<IMeshBuffer*> m_meshBuffers;
};

