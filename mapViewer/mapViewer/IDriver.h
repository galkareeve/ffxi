#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class IMeshBuffer;
class IDriver
{
public:
	IDriver(void){};
	virtual ~IDriver(void){};

	virtual unsigned int createTexture(unsigned int width, unsigned int height, unsigned int mipMapCount, unsigned char *pImg)=0;
	virtual unsigned int createDDSTexture(int type, glm::u32 width, glm::u32 height, glm::u32 mipMapCount, glm::u8 *pImg) = 0;
	virtual void deleteTexture(unsigned int tid)=0;
	virtual void draw(int, IMeshBuffer *, int bflg=0, int usealpha=0)=0;
	virtual void drawCube(int frame, IMeshBuffer *mb)=0;
	virtual GLuint selectProgramID( int s)=0;
	virtual void createMatrixHandler()=0;
	virtual void initProjectionMatrix(glm::mat4 &ModelMatrix, glm::mat4 &ViewMatrix, glm::mat4 &MVP)=0;
};

