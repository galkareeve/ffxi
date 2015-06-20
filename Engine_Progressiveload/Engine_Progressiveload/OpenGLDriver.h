#pragma once
#include "idriver.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

class COpenGLDriver :	public IDriver
{
public:
	COpenGLDriver(void);
	virtual ~COpenGLDriver(void);

	unsigned int createTexture(unsigned int width, unsigned int height, unsigned int mipMapCount, unsigned char *pImg);
	void draw(int frame, IMeshBuffer *mb);
	void setProgramID( GLuint pid );
	void setShaderTextureID(GLuint sid);
	void assignGLBufferID(GLuint vb, GLuint uv, GLuint nor);
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

