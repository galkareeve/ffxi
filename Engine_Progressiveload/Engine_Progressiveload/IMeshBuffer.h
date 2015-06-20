#pragma once
#include "myEnum.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>

struct SFrameBuffer
{
	bool isLoaded;
	bool isLock;		//lock when writing, since Mesh can be shared by multiply sceneNode
	std::vector<glm::vec3> m_vecVertices;
	std::vector<glm::vec3> m_vecNormal;
};

class IMeshBuffer
{
public:
	IMeshBuffer(GL_DRAWTYPE dt) {m_gldrawType=dt;};
	virtual ~IMeshBuffer(void);

	void clear();
	void generateFrameBuffer(unsigned int size);
	bool isFrameBufferLoaded(int frame);
	bool isFrameBufferLock(int frame);
	void lockFrameBuffer(int frame);
	void unlockFrameBuffer(int frame);

	GL_DRAWTYPE getDrawType() { return m_gldrawType; };
	void updateTextureID(unsigned int id) { m_textureID=id; };
	unsigned int getTextureID() { return m_textureID; };

	void updateVertexBuffer(unsigned int frame, std::vector<glm::vec3> &vert);
	void updateNormalBuffer(unsigned int frame, std::vector<glm::vec3> &normal);
	void updateIndicesBuffer(std::vector<glm::u16> &indices);
	void updateUVBuffer(std::vector<glm::vec2> &uv);
//	void updateBuffer(std::vector<glm::vec3> &vert, std::vector<glm::vec3> &normal, std::vector<glm::u16> &indices, std::vector<glm::vec2> &uv);

	void outputMeshInfo(int frame);

	GL_DRAWTYPE m_gldrawType;
	unsigned int m_textureID;

	std::vector<glm::u16> m_vecIndices;
	std::vector<glm::vec2> m_vecUV;
	std::vector<SFrameBuffer*> m_vecFrameBuffer;
};

