#include "OpenGLDriver.h"

#include "IMeshBuffer.h"

COpenGLDriver::COpenGLDriver(void)
{
}


COpenGLDriver::~COpenGLDriver(void)
{
}

void COpenGLDriver::setProgramID( GLuint pid, GLuint pidcube )
{
	m_programID = pid;
	m_programIDcube = pidcube;
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pImg);

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

void COpenGLDriver::draw(int frame, IMeshBuffer *mb)
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

		// Draw the triangles !
		if( mb->m_gldrawType==E_TRIANGLE_LIST) {
			glDrawArrays(GL_TRIANGLES, 0, mb->m_vecFrameBuffer[frame]->m_vecVertices.size() );
		}
		else {
			glDrawArrays(GL_TRIANGLE_STRIP, 0, mb->m_vecFrameBuffer[frame]->m_vecVertices.size() );
		}

		// Index buffer VBO, not suitable for ffxi, since it uses indice to retrieve vertex which is packed with uv ==> same as irrlicht S3DVertex....
		// Draw the triangles !
		//if( mb->m_gldrawType==E_TRIANGLE_LIST) {
		//	glDrawElements(
		//		GL_TRIANGLES,      // mode
		//		mb->m_vecIndices.size(),    // count
		//		GL_UNSIGNED_SHORT, // type
		//		(void*)0           // element array buffer offset
		//	);
		//}
		//else if(mb->m_gldrawType==E_TRIANGLE_STRIP) {
		//	glDrawElements(
		//		 GL_TRIANGLE_STRIP,      // mode
		//		mb->m_vecIndices.size(),    // count
		//		GL_UNSIGNED_SHORT, // type
		//		(void*)0           // element array buffer offset
		//	);
		//}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
}

void COpenGLDriver::assignGLBufferID(GLuint vb, GLuint uv, GLuint nor, GLuint col)
{
	vertexbuffer = vb;
	uvbuffer = uv;
	normalbuffer = nor;
	colorbuffer = col;
}

void COpenGLDriver::cleanUp()
{
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
}
