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
	unsigned int createDDSTexture(int type, glm::u32 width, glm::u32 height, glm::u32 mipMapCount, glm::u8 *pImg);
	void deleteTexture(unsigned int tid);
	void draw(int frame, IMeshBuffer *mb, int bflg, int useAlpha = 0);
	void drawCube(int frame, IMeshBuffer *mb);
	void setProgramID( GLuint pid, GLuint pidcube );
	GLuint selectProgramID( int s);
	void setShaderTextureID(GLuint sid);
	void assignGLBufferID(GLuint vb, GLuint uv, GLuint nor, GLuint col);
	void cleanUp();

	void createMatrixHandler();
	void initProjectionMatrix(glm::mat4 &ModelMatrix, glm::mat4 &ViewMatrix, glm::mat4 &MVP);

	void toggleWireframe();
	bool isWireframe() { return m_isWireframe; }

protected:
	GLuint m_programID, m_programIDcube;
	GLuint m_shaderTextureID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint colorbuffer;			//for cube shader
	GLuint elementbuffer;		//for index VBO

	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint MVPID;		//for cube shader
	GLuint MultiplerID;
	GLuint useAlphaID;
	GLuint MaterialColorID;

	bool m_isWireframe;
};

