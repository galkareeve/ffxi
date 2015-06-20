#pragma once
#include <glm/glm.hpp>

class IMeshBuffer;
class IDriver
{
public:
	IDriver(void){};
	virtual ~IDriver(void){};

	virtual unsigned int createTexture(unsigned int width, unsigned int height, unsigned int mipMapCount, unsigned char *pImg)=0;
	virtual void draw(int, IMeshBuffer *)=0;
	virtual void createMatrixHandler()=0;
	virtual void initProjectionMatrix(glm::mat4 &ModelMatrix, glm::mat4 &ViewMatrix, glm::mat4 &MVP)=0;
};

