#pragma once
#include "imesh.h"
#include <vector>

struct STextureInfo {
	unsigned int id;
	std::string name;
};

class FFXIParts;
class IDriver;
class CFFXI_Character;
class IMeshBuffer;
class CFFXIMesh :	public IMesh
{
public:
	CFFXIMesh(IDriver *in);
	virtual ~CFFXIMesh(void);
	void recalculateBoundingBox();

	bool loadModelFile(std::string FN);
	void prepareFrameBuffer();
	bool isFrameFullyLoaded();
	bool animate(int frame);
	void addMeshBuffer(IMeshBuffer *in) {m_meshBuffers.push_back(in); };
	void updateMeshBuffer(unsigned int i, unsigned int pn, IMeshBuffer *inout);
	int getMeshBufferCount() { return m_meshBuffers.size(); };
	IMeshBuffer* getMeshBuffer(unsigned int i);

	int getAnimIndex(int frameStart, int frameEnd);
	void nextAnimation(float &speed, int &frameStart, int &frameEnd);
	void setCurrentAnimation(int i, int &frameStart, int &frameEnd, float &speed);

private:
	IDriver *p_driver;
	CFFXI_Character *p_mFFChar;

	int m_totalAnimationFrame;		//sum of all frame from noAnimation
	int m_numAnimation;				//no of unique animation
	int m_numFrameLoaded;			//num of frame loaded
	std::vector<IMeshBuffer*> m_meshBuffers;		//all the individual parts
	std::vector<unsigned int> m_mappingMesh2Parts;
};

