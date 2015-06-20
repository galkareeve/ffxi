#include "IMeshBuffer.h"
#include <iostream>
#include <fstream>

IMeshBuffer::~IMeshBuffer(void)
{
}

void IMeshBuffer::clear()
{

}

void IMeshBuffer::generateFrameBuffer(unsigned int size)
{
	for(int i=0; i<size; ++i) {
		SFrameBuffer *fb = new SFrameBuffer;
		fb->isLoaded=false;
		fb->isLock=false;
		m_vecFrameBuffer.push_back(fb);
	}
}

bool IMeshBuffer::isFrameBufferLoaded(int frame)
{
	return m_vecFrameBuffer[frame]->isLoaded;
}

bool IMeshBuffer::isFrameBufferLock(int frame)
{
	return m_vecFrameBuffer[frame]->isLock;
}

void IMeshBuffer::lockFrameBuffer(int frame)
{
	m_vecFrameBuffer[frame]->isLock=true;
}

void IMeshBuffer::unlockFrameBuffer(int frame)
{
	m_vecFrameBuffer[frame]->isLock=false;
}

void IMeshBuffer::updateVertexBuffer(unsigned int frame, std::vector<glm::vec3> &vert)
{
	m_vecFrameBuffer[frame]->m_vecVertices.clear();
	for(auto it=vert.begin(); it!=vert.end(); ++it) {
		m_vecFrameBuffer[frame]->m_vecVertices.push_back(*it);
	}
}

void IMeshBuffer::updateNormalBuffer(unsigned int frame, std::vector<glm::vec3> &normal)
{
	m_vecFrameBuffer[frame]->m_vecNormal.clear();
	for(auto it=normal.begin(); it!=normal.end(); ++it)
		m_vecFrameBuffer[frame]->m_vecNormal.push_back(*it);

	m_vecFrameBuffer[frame]->isLoaded=true;
}

void IMeshBuffer::updateIndicesBuffer(std::vector<glm::u16> &indices)
{
	m_vecIndices.clear();
	m_vecIndices = indices;
}

void IMeshBuffer::updateUVBuffer(std::vector<glm::vec2> &uv)
{
	m_vecUV.clear();
	m_vecUV = uv;
}

//void IMeshBuffer::updateBuffer(std::vector<glm::vec3> &vert, std::vector<glm::vec3> &normal, std::vector<glm::u16> &indices, std::vector<glm::vec2> &uv)
//{
//	m_vecVertices = vert;
//	m_vecNormal = normal;
//	m_vecIndices = indices;
//	m_vecUV = uv;
//}

void IMeshBuffer::outputMeshInfo(int frame)
{
	int i=1;
	std::ofstream ofs( "debug_mesh.txt", std::ios::out | std::ios::app);
	auto nit=m_vecIndices.begin();
	ofs << "Frame: " << frame << std::endl;
	for(auto it=m_vecFrameBuffer[frame]->m_vecVertices.begin(); it!=m_vecFrameBuffer[frame]->m_vecVertices.end(); ++it,++i,++nit) {
		ofs << i << ") Indice: " << (*nit) << "  [" << (*it).x <<"," << (*it).y << "," << (*it).z << "]  " << std::endl;
	}

	ofs.close();
}
