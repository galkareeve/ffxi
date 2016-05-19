#include "OpenGLDriver.h"

#include "IMeshBuffer.h"

COpenGLDriver::COpenGLDriver(void)
{
}


COpenGLDriver::~COpenGLDriver(void)
{
}

void COpenGLDriver::setProgramID( GLuint pid )
{
	m_programID = pid;
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
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pImg);

	return textureID;

}

void COpenGLDriver::draw(int frame, IMeshBuffer *mb)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, mb->m_vecFrameBuffer[frame]->m_vecVertices.size() * sizeof(glm::vec3), &mb->m_vecFrameBuffer[frame]->m_vecVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, mb->m_vecUV.size() * sizeof(glm::vec2), &mb->m_vecUV[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, mb->m_vecFrameBuffer[frame]->m_vecNormal.size() * sizeof(glm::vec3), &mb->m_vecFrameBuffer[frame]->m_vecNormal[0], GL_STATIC_DRAW);
	
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mb->m_vecIndices.size() * sizeof(glm::u16), &mb->m_vecIndices[0], GL_STATIC_DRAW);


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
			0,                  // attribute
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
			1,                                // attribute
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
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
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

void COpenGLDriver::assignGLBufferID(GLuint vb, GLuint uv, GLuint nor)
{
	vertexbuffer = vb;
	uvbuffer = uv;
	normalbuffer = nor;
	
	//Create default vertex array
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void COpenGLDriver::cleanUp()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(m_programID);
}

void COpenGLDriver::initProjectionMatrix(glm::mat4 &ModelMatrix, glm::mat4 &ViewMatrix, glm::mat4 &MVP)
{
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
}

void COpenGLDriver::createMatrixHandler()
{
	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(m_programID, "MVP");
	ViewMatrixID = glGetUniformLocation(m_programID, "V");
	ModelMatrixID = glGetUniformLocation(m_programID, "M");

}
