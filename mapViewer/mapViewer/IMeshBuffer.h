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
	IMeshBuffer(GL_DRAWTYPE dt);
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

	void addVertexBuffer(unsigned int frame, glm::vec3 &in) {m_vecFrameBuffer[frame]->m_vecVertices.push_back(in);}
	void addNormalBuffer(unsigned int frame, glm::vec3 &in) {m_vecFrameBuffer[frame]->m_vecNormal.push_back(in);}
	void addIndicesBuffer(glm::u16 indices) {m_vecIndices.push_back(indices);}
	void addUVBuffer(glm::vec2 &in) {m_vecUV.push_back(in);}
	void addVertexColor(glm::vec4 color) { m_vecColor.push_back(color);}
	void updateVertexBuffer(unsigned int frame, std::vector<glm::vec3> &vert);
	void updateNormalBuffer(unsigned int frame, std::vector<glm::vec3> &normal);
	void updateIndicesBuffer(std::vector<glm::u16> &indices);
	void updateUVBuffer(std::vector<glm::vec2> &uv);
	void updateColorBuffer(std::vector<glm::vec4> &color);
//	void updateBuffer(std::vector<glm::vec3> &vert, std::vector<glm::vec3> &normal, std::vector<glm::u16> &indices, std::vector<glm::vec2> &uv);
	void setBlendFlag(unsigned int bflg, unsigned int useAlpha) { m_multipler = bflg; m_useAlpha = useAlpha; };
	unsigned int getMultipler() { return m_multipler; };
	unsigned int isUseAlpha() { return m_useAlpha; };

	void outputMeshInfo(int frame, int mbIndex);

	GL_DRAWTYPE m_gldrawType;
	unsigned int m_textureID;
	unsigned int m_multipler;
	unsigned int m_useAlpha;

	std::vector<glm::u16> m_vecIndices;
	std::vector<glm::vec2> m_vecUV;
	std::vector<glm::vec4> m_vecColor;			//vertex color
	std::vector<SFrameBuffer*> m_vecFrameBuffer;

};

