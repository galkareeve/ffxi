#include "MeshBufferGroup.h"


CMeshBufferGroup::CMeshBufferGroup(int mzb, int mmb) : m_MZBIndex(mzb), m_MMBIndex(mmb)
{
	m_timeLastDrawn=0;
	m_meshBuffers.clear();
}


CMeshBufferGroup::~CMeshBufferGroup(void)
{
	for(auto it=m_meshBuffers.begin(); it!=m_meshBuffers.end(); ++it)
		delete *it;

	m_meshBuffers.clear();
}

IMeshBuffer* CMeshBufferGroup::getMeshBuffer(unsigned int i)
{
	return m_meshBuffers[i];
}
