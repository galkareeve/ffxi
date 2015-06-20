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
	void deleteTexture(unsigned int tid);
	void draw(int frame, IMeshBuffer *mb);
	void drawCube(int frame, IMeshBuffer *mb);
	void setProgramID( GLuint pid, GLuint pidcube );
	GLuint selectProgramID( int s);
	void setShaderTextureID(GLuint sid);
	void assignGLBufferID(GLuint vb, GLuint uv, GLuint nor, GLuint col);
	void cleanUp();

	void createMatrixHandler();
	void initProjectionMatrix(glm::mat4 &ModelMatrix, glm::mat4 &ViewMatrix, glm::mat4 &MVP);

protected:
	GLuint m_programID, m_programIDcube;
	GLuint m_shaderTextureID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint colorbuffer;			//for cube shader

	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint MVPID;		//for cube shader
};

