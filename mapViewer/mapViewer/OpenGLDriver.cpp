#include "OpenGLDriver.h"

#include "IMeshBuffer.h"
#include <iostream>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

struct DdsLoadInfo {
	bool compressed;
	bool swap;
	bool palette;
	unsigned int divSize;
	unsigned int blockBytes;
	GLenum internalFormat;
	GLenum externalFormat;
	GLenum type;
};

DdsLoadInfo loadInfoDXT1 = {
	true, false, false, 4, 8, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
};
DdsLoadInfo loadInfoDXT3 = {
	true, false, false, 4, 16, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
};
DdsLoadInfo loadInfoDXT5 = {
	true, false, false, 4, 16, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};
DdsLoadInfo loadInfoBGRA8 = {
	false, false, false, 1, 4, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE
};
DdsLoadInfo loadInfoBGR8 = {
	false, false, false, 1, 3, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE
};
DdsLoadInfo loadInfoBGR5A1 = {
	false, true, false, 1, 2, GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV
};
DdsLoadInfo loadInfoBGR565 = {
	false, true, false, 1, 2, GL_RGB5, GL_RGB, GL_UNSIGNED_SHORT_5_6_5
};
DdsLoadInfo loadInfoIndex8 = {
	false, false, true, 1, 1, GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE
};

COpenGLDriver::COpenGLDriver(void)
{
	m_isWireframe = false;
}


COpenGLDriver::~COpenGLDriver(void)
{
}

void COpenGLDriver::setProgramID( GLuint pid, GLuint pidcube )
{
	m_programID = pid;
	m_programIDcube = pidcube;
}

void COpenGLDriver::toggleWireframe()
{
	m_isWireframe = !m_isWireframe;
	if (m_isWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
GLuint COpenGLDriver::selectProgramID( int s)
{
	if(s==1) {
		glUseProgram(m_programIDcube);
		return m_programIDcube;
	}
	else
		glUseProgram(m_programID);
	return m_programID;
}

void COpenGLDriver::setShaderTextureID( GLuint sid)
{
	m_shaderTextureID = sid;
}

unsigned int COpenGLDriver::createTexture(glm::u32 width, glm::u32 height, glm::u32 mipMapCount, glm::u8 *pImg)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
//	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//GL_CLAMP_TO_EDGE will strecth the last pixel of the texture to fill the polygon
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );

	//the last 3 param describe how the image is represented in memory (DDS2BMP conversion)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pImg);
	return textureID;
}

unsigned int COpenGLDriver::createDDSTexture(int type, glm::u32 width, glm::u32 height, glm::u32 mipMapCount, glm::u8 *pImg)
{
	size_t s = 0;
	unsigned int x = width;
	unsigned int y = height;

	DdsLoadInfo * li;
	switch (type)
	{
	case 1:		li = &loadInfoDXT1;
		break;
	case 3:		li = &loadInfoDXT3;
		break;
	case 5:		li = &loadInfoDXT5;
		break;
	default :
		std::cout << "Unknown DDS type: " << type << std::endl;
		return 0;
	}

	size_t size = max(li->divSize, x) / li->divSize * max(li->divSize, y) / li->divSize * li->blockBytes;
	if (!pImg) {
		return 0;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);

	for (unsigned int ix = 0; ix < mipMapCount; ++ix) {
		glCompressedTexImage2D(GL_TEXTURE_2D, ix, li->internalFormat, x, y, 0, size, (GLvoid*)pImg);
		x = (x + 1) >> 1;
		y = (y + 1) >> 1;
		size = max(li->divSize, x) / li->divSize * max(li->divSize, y) / li->divSize * li->blockBytes;
	}

	return textureID;
}

void COpenGLDriver::deleteTexture(unsigned int tid)
{
	glDeleteTextures(1, &tid);
}

void COpenGLDriver::drawCube(int frame, IMeshBuffer *mb)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, mb->m_vecFrameBuffer[frame]->m_vecVertices.size() * sizeof(glm::vec3), &mb->m_vecFrameBuffer[frame]->m_vecVertices[0], GL_STATIC_DRAW);

	//can store the color info in the normal array since it is not used!
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, mb->m_vecFrameBuffer[frame]->m_vecNormal.size() * sizeof(glm::vec3), &mb->m_vecFrameBuffer[frame]->m_vecNormal[0], GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the Line
	if( mb->m_gldrawType==E_LINE)
		glDrawArrays(GL_LINES, 0, mb->m_vecFrameBuffer[frame]->m_vecVertices.size() );
	else if(mb->m_gldrawType==E_LINE_LOOP)
		glDrawArrays(GL_LINE_LOOP, 0, mb->m_vecFrameBuffer[frame]->m_vecVertices.size() );

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void COpenGLDriver::draw(int frame, IMeshBuffer *mb, int multipler, int useAlpha)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, mb->m_vecFrameBuffer[frame]->m_vecVertices.size() * sizeof(glm::vec3), &mb->m_vecFrameBuffer[frame]->m_vecVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, mb->m_vecUV.size() * sizeof(glm::vec2), &mb->m_vecUV[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, mb->m_vecFrameBuffer[frame]->m_vecNormal.size() * sizeof(glm::vec3), &mb->m_vecFrameBuffer[frame]->m_vecNormal[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mb->m_vecColor.size() * sizeof(glm::vec4), &mb->m_vecColor[0], GL_STATIC_DRAW);


	// Bind our texture in Texture Unit 0
	GLuint TextureID = mb->getTextureID();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	// Set our "myTextureSampler" sampler to user Texture Unit 0 [Texture Unit refer to multi-texture sampling, eg, diffuse, bump, displacement, lightmap...etc]
	glUniform1i(m_shaderTextureID, 0);

	glUniform1i(MultiplerID, multipler);
	glUniform1i(useAlphaID, useAlpha);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
			0,                  // location
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // location
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // location
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 4th attribute buffer : color
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			3,                                // location in vertexShader
			4,                                // size
			GL_FLOAT,					      // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		//// Draw the triangles !
		//if( mb->m_gldrawType==E_TRIANGLE_LIST) {
		//	glDrawArrays(GL_TRIANGLES, 0, mb->m_vecFrameBuffer[frame]->m_vecVertices.size() );
		//}
		//else {
		//	glDrawArrays(GL_TRIANGLE_STRIP, 0, mb->m_vecFrameBuffer[frame]->m_vecVertices.size() );
		//}

		// Generate a buffer for the indices as well
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mb->m_vecIndices.size() * sizeof(unsigned short), &mb->m_vecIndices[0], GL_STATIC_DRAW);

		 //Draw the triangles !
		if( mb->m_gldrawType==E_TRIANGLE_LIST) {
			glDrawElements(
				GL_TRIANGLES,      // mode
				mb->m_vecIndices.size(),    // count
				GL_UNSIGNED_SHORT, // type
				(void*)0           // element array buffer offset
			);
		}
		else if(mb->m_gldrawType==E_TRIANGLE_STRIP) {
			glDrawElements(
				 GL_TRIANGLE_STRIP,      // mode
				mb->m_vecIndices.size(),    // count
				GL_UNSIGNED_SHORT, // type
				(void*)0           // element array buffer offset
			);
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
}

void COpenGLDriver::assignGLBufferID(GLuint vb, GLuint uv, GLuint nor, GLuint col)
{
	vertexbuffer = vb;
	uvbuffer = uv;
	normalbuffer = nor;
	colorbuffer = col;

	glGenBuffers(1, &elementbuffer);
}

void COpenGLDriver::cleanUp()
{
	glDeleteBuffers(1, &elementbuffer);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(m_programID);
	glDeleteProgram(m_programIDcube);
}

void COpenGLDriver::initProjectionMatrix(glm::mat4 &ModelMatrix, glm::mat4 &ViewMatrix, glm::mat4 &MVP)
{
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	//for cube shader, cant assign variable to other programID that is not active
//	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
}

void COpenGLDriver::createMatrixHandler()
{
	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(m_programID, "MVP");
	ViewMatrixID = glGetUniformLocation(m_programID, "V");
	ModelMatrixID = glGetUniformLocation(m_programID, "M");
	//for cube shader
	MVPID = glGetUniformLocation(m_programIDcube, "MVP");

	MultiplerID = glGetUniformLocation(m_programID, "iMultipler");
	useAlphaID = glGetUniformLocation(m_programID, "iuseAlpha");
//	MaterialColorID = glGetUniformLocation(m_programID, "iMatColor");
}
