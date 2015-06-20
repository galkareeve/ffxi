#include "IMeshBuffer.h"
#include <iostream>
#include <fstream>

IMeshBuffer::IMeshBuffer(GL_DRAWTYPE dt)
{
	m_gldrawType=dt;
	m_textureID=0;
}

IMeshBuffer::~IMeshBuffer(void)
{
	m_vecIndices.clear();
	m_vecUV.clear();
	for(auto it=m_vecFrameBuffer.begin(); it!=m_vecFrameBuffer.end(); ++it) {
		(*it)->m_vecNormal.clear();
		(*it)->m_vecVertices.clear();
		delete *it;
	}
	m_vecFrameBuffer.clear();
}

void IMeshBuffer::clear()
{
	//clear the vertex/normal ONLY, if need to clear indice/uv, then might as well delete the meshBuffer
	for(auto it=m_vecFrameBuffer.begin(); it!=m_vecFrameBuffer.end(); ++it) {
		(*it)->m_vecNormal.clear();
		(*it)->m_vecVertices.clear();
		(*it)->isLoaded=false;
	}
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
	for(auto it=vert.begin(); it!=vert.end(); ++it)
		m_vecFrameBuffer[frame]->m_vecVertices.push_back(*it);
		
	//loaded only if vertex is updated
	m_vecFrameBuffer[frame]->isLoaded=true;
}

void IMeshBuffer::updateNormalBuffer(unsigned int frame, std::vector<glm::vec3> &normal)
{
	m_vecFrameBuffer[frame]->m_vecNormal.clear();
	for(auto it=normal.begin(); it!=normal.end(); ++it)
		m_vecFrameBuffer[frame]->m_vecNormal.push_back(*it);
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

void IMeshBuffer::updateColorBuffer(std::vector<glm::vec4> &color)
{
	m_vecColor.clear();
	m_vecColor = color;
}

//void IMeshBuffer::updateBuffer(std::vector<glm::vec3> &vert, std::vector<glm::vec3> &normal, std::vector<glm::u16> &indices, std::vector<glm::vec2> &uv)
//{
//	m_vecVertices = vert;
//	m_vecNormal = normal;
//	m_vecIndices = indices;
//	m_vecUV = uv;
//}

void IMeshBuffer::outputMeshInfo(int frame, int mbIndex)
{
	int i=0;
	char buf[100];
	sprintf_s(buf,100,"meshBuffer_%d_%d", frame, mbIndex);

	std::ofstream ofs( buf, std::ios::out | std::ios::app);
	//auto nit=m_vecIndices.begin();
	//ofs << "Frame: " << frame << std::endl;
	//for(auto it=m_vecFrameBuffer[frame]->m_vecVertices.begin(); it!=m_vecFrameBuffer[frame]->m_vecVertices.end(); ++it,++i,++nit) {
	//	ofs << i << ") Indice: " << (*nit) << "  [" << (*it).x <<"," << (*it).y << "," << (*it).z << "]  " << std::endl;
	//}
	//i=0;
	//for(auto it=m_vecFrameBuffer[frame]->m_vecNormal.begin(); it!=m_vecFrameBuffer[frame]->m_vecNormal.end(); ++it,++i) {
	//	ofs << i << ") Normal:  [" << (*it).x <<"," << (*it).y << "," << (*it).z << "]  " << std::endl;
	//}

	//i=0;
	//for(auto it=m_vecUV.begin(); it!=m_vecUV.end(); ++it, ++i) {
	//	ofs << i << ") UV: [" << (*it).x << "," << (*it).y << "] " << std::endl;
	//}

	ofs << m_vecIndices.size()/3 << std::endl;
	for(auto it=m_vecIndices.begin(); it!=m_vecIndices.end();) {
		ofs << (*it) << ",";
		++it;
		ofs << (*it) << ",";
		++it;
		ofs << (*it) << std::endl;
		++it;
	}
	ofs << m_vecFrameBuffer[0]->m_vecVertices.size() << std::endl;
	for(auto vit=m_vecFrameBuffer[0]->m_vecVertices.begin(); vit!=m_vecFrameBuffer[0]->m_vecVertices.end(); ++vit) {
		ofs << (*vit).x << "," << (*vit).y << "," << (*vit).z << std::endl;
	}
	ofs.close();
}
