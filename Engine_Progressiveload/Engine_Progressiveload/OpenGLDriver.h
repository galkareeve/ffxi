#pragma once
#include "idriver.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

class COpenGLDriver :	public IDriver
{
public:
	COpenGLDriver(void);
	virtual ~COpenGLDriver(void);

	unsigned int createTexture(glm::u32 width, glm::u32 height, glm::u32 mipMapCount, glm::u8 *pImg);
	void draw(int frame, IMeshBuffer *mb);
	void setProgramID( GLuint pid );
	void setShaderTextureID(GLuint sid);
	void assignGLBufferID();
	void cleanUp();

	void createMatrixHandler();
	void initProjectionMatrix(glm::mat4 &ModelMatrix, glm::mat4 &ViewMatrix, glm::mat4 &MVP);

protected:
	GLuint m_programID;
	GLuint m_shaderTextureID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;

	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
};

